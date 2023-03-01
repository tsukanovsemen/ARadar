#include "arkan_processing.h"
#include <math.h>
//#include "micran_fft.h"
//#include "abstract_arkan_processing_observer.h"
//#include "empirical.h"
//#include "sentry.h"
//#include "IPPI.h"
//#include "angular.h"
//#include "mutextrylocker.h"
//#include "implementation/ssepowlog.h"
//#include "socket_tcp_slave_pkg.h"
#include <QtDebug>
#include "mymath.h"

ArkanProcessing::ArkanProcessing()
{
    _srcComplexI32.resize(SRC_SIZE);
    _srcAbsFlt.resize(SRC_SIZE);
    _fftOutputBuffer.resize(SRC_SIZE);
    _bufferFftComplexFlt.resize(SRC_SIZE);
//    _window.resize(SRC_SIZE);
    _srcComplexFlt.resize(SRC_SIZE);

    _fftComplex.resize(FFT_SIZE);
    _fftAbs.resize(FFT_SIZE);
    _8bitBuffer.resize(FFT_SIZE);
    _2bitbuffer.resize(FFT_SIZE);
    _thresholdLine.resize(FFT_SIZE, 1);
    _logBuffer.resize(FFT_SIZE);

//    log2_init();
//    exp2_init();

    std::fill(_thresholdLine.begin(), _thresholdLine.end(), _threshold);

//    _spotCombiner = std::make_unique<SpotCombiner>([this](auto angle, auto &targets) {
//        const double ppiAzimuth = _ppi->azimuth();
//        const double spokeAzimuth = Angular::rotate(angle, ppiAzimuth);

//        MarkList markList;
//        markList.reserve(targets.size());

//        for (const auto &target : targets) {
//            const auto targetAzimuth = Angular::rotate(target.angle, ppiAzimuth);

//            Mark mark;
//            {
//                const auto targetAngle = targetAzimuth * M_PI / 180.;
//                const auto distance =
//                 std::sqrt(std::pow(target.distance, 2) - std::pow(_antennaHeight, 2));

//                mark.x = std::cos(targetAngle) * distance;
//                mark.y = std::sin(targetAngle) * distance;
//                mark.intensity = target.intensity;
//                mark.size = target.size;
//                mark.ppi = _ppi;
//            }

//            markList.push_back(std::move(mark));
//        }

//        const auto sentry = qobject_cast<Sentry *>(_sentry);

//        if (!sentry)
//            return;

//        sentry->pushMarks(spokeAzimuth, markList);
//    });

//    connect(_ppi, &IPPI::azimuthChanged, this, [this](const auto azimuth) {
//        _zoneFilter.setAngleShift(RadarCommon::fromDegrees(azimuth));
//    });

//    _spotCombiner->setRange(_ppi->range());

//    setGain(0);
}

void ArkanProcessing::addObserver(AbstractArkanProcessingObserver *observer)
{
    //QMutexLocker locker(&_mutex);

    if (_observers.contains(observer))
        return;

    _observers.push_back(observer);
}

void ArkanProcessing::removeObserver(AbstractArkanProcessingObserver *observer)
{
    QMutexLocker locker(&_mutex);

    _observers.removeOne(observer);
}

//void ArkanProcessing::processStreamData(const std::vector<char> &data)
//{
//    // Необходима блокировка, чтобы не было гонки среди обработки данных
//    QMutexLocker locker(&_mutex);

//    foreach (const auto observer, _observers)
//        observer->notifyRawData(data);

    //nArcn::sFrameHead *header =
    // reinterpret_cast<nArcn::sFrameHead *>(const_cast<char *>(data.data()));

    //const unsigned lineSize = header->CountPntRow * header->CountPntCol;

//    const auto rawData =
//     reinterpret_cast<void *>(const_cast<char *>(data.data() + sizeof(nArcn::sFrameHead)));

//    const auto angle = header->Pos;
//    const auto dataType = header->TpDtOut;

//    switch (dataType) {
//    case nArcn::eTpDtOut_CmplxS32: // Без обработки
//        processRawCmplxI32(angle, rawData, lineSize);
//        break;
//    case nArcn::eTpDtOut_CmplxFlt: // Без обработки
//        processCmplxFlt(angle, rawData, lineSize);
//        break;
//    case nArcn::eTpDtOut_FltFft: // Быстрое преобразование Фурье
//        processCmplxFFT(angle, rawData, lineSize);
//        break;
//    case nArcn::eTpDtOut_Target: // Отметки
//        processTargets(angle, nullptr, 0);
//        break;
//    default:
//        qWarning() << "Unknown command for source data from radar";
//        break;
//    }
//}

void ArkanProcessing::processRawCmplxI32(unsigned angle, const void *data, size_t size) noexcept
{
    _mutex.tryLock();
    if (!_mutex.tryLock())
        return;

    _lineSize = size;
    memcpy(_srcComplexI32.data(), data, size * sizeof(ComplexI32));

    //foreach (const auto observer, _observers)
    //    observer->notifyRawCmplxI32(angle, _srcComplexI32.data());

    MyMath::Hypot(&_srcComplexI32, &_srcAbsFlt);

    //foreach (const auto observer, _observers)
    //      observer->notifySrc(angle, _srcAbsFlt.data());

    // Выполняем преобразование FFT
    convertSrcToFft(_srcComplexI32.data(), _fftOutputBuffer.data(), size);

    // Выполняем преобразование в LOG
    processCmplxFFT(angle, _fftOutputBuffer.data(), size / 2);
}

void ArkanProcessing::processCmplxFlt(unsigned angle, const void *data, size_t size) noexcept
{
    _mutex.tryLock();

    if (!_mutex.tryLock())
        return;

    _lineSize = size;
    memcpy(_srcComplexFlt.data(), data, size * sizeof(ComplexF));

//    foreach (const auto observer, _observers)
//        observer->notifyRawCmplxFlt(angle, _srcComplexFlt.data());

    MyMath::Hypot(&_srcComplexFlt, &_srcAbsFlt);
//    RAlg::hypot(_srcComplexFlt.data(), _srcAbsFlt.data(), size);

//    foreach (const auto observer, _observers)
//        observer->notifySrc(angle, _srcAbsFlt.data());

    // Выполняем преобразование FFT
    convertSrcToFft(_srcComplexFlt.data(), _fftOutputBuffer.data(), size);

    // Выполняем преобразование в LOG
    processCmplxFFT(angle, _fftOutputBuffer.data(), size / 2);
}

void ArkanProcessing::processCmplxFFT(unsigned angle, const void *data, size_t size) noexcept
{
    _mutex.tryLock();
    if (!_mutex.tryLock())
        return;

    _lineSize = size;

    //complexI32 -> complexF
    memcpy(_fftComplex.data(), data, size * sizeof(ComplexI32));

//    foreach (const auto observer, _observers)
//        observer->notifyRawCmplxFFT(angle, _fftComplex.data());

    MyMath::Hypot(&_fftComplex, &_fftAbs);
//    RAlg::hypot(_fftComplex.data(), _fftAbs.data(), size);

//    _rain.apply(_fftAbs.data());
//    _sea.apply(_fftAbs.data());
//    _gain.apply(_fftAbs.data());

//    if (_ground2d->haveThresholds() && _zoneFilter.canApply())
//        _zoneFilter.apply(angle, _fftAbs.data(), _ground2d->threshold(angle).data());

    /// TODO:: Уточнить нужно ли это добавлять в гард
    /// если да, то просто убрать ифдеф
    /// если нет, то добавить макрос, который не зависит от названия проекта, а от плагина
#ifdef RADAR_CONSOLE
    _expanding.apply(_fftAbs.data());
    _rcalc.postProcessFft(_fftAbs.data());

    const float bs = RAlg::contrastToB(_contrast);
    std::transform(_fftAbs.data(), _fftAbs.data() + size, _logBuffer.begin(), [bs](float x) {
        x = RAlg::TableLog::log(x, RAlg::A, bs, 0);
        return std::min(x, 255.f);
    });
#endif
//    // Накопление данных для 2-битки
//    if (const auto thresholdsIsntReady = !_ground2d->haveThresholds();
//        thresholdsIsntReady || _ground2d->adaptiveThreshold()) {
//        _ground2d->push(_fftAbs.data(), angle);

//        if (thresholdsIsntReady)
//            emit accumulationPercentChanged(_ground2d->accumulationPercent());
//     S}

    convertFftToLog(_fftAbs.data(), _8bitBuffer.data(), size);

//     std::vector<float> bit8;
//     bit8.resize(LINE_SIZE);
//     std::copy(_8bitBuffer.begin(), _8bitBuffer.end(), bit8.begin());

    // QMetaObject::invokeMethod(this, [this, angle, bit8]() {
    // Передаём всем observer'ам 8-битку
//    foreach (const auto observer, _observers)
//        observer->notify8bit(angle, _8bitBuffer.data());
//    //});

//    if (_ground2d->haveThresholds()) {
//        if (_groundState != State::Processing)
//            _groundState = State::Processing;

//        emit radarWorked();

        _2bitbuffer = _fftAbs;

//        foreach (const auto observer, _observers)
//            observer->notifyGround(angle, _ground2d->threshold(angle).data());

//        RAlg::thresholdSubtract(_2bitbuffer.data(), _2bitbuffer.data(),
//                                _ground2d->threshold(angle).data(), FFT_SIZE);

//        foreach (const auto observer, _observers)
//            observer->notifySubtract(angle, _2bitbuffer.data());

//        // Искуственное завышение порогов
///        RAlg::thresholdSubtract(_2bitbuffer.data(), _2bitbuffer.data(), _thresholdLine.data(),
///                                FFT_SIZE);

//        foreach (const auto observer, _observers)
//            observer->notifyThreshold(angle, _2bitbuffer.data());

//        if (sbRatio(angle, _2bitbuffer.data()) < 1.1f)
//            _spotCombiner->push(angle, _2bitbuffer.data());

        convertFftToLog(_2bitbuffer.data(), _2bitbuffer.data(), size);

//        // Передаём всем observer'ам 2-битку
//        foreach (const auto observer, _observers)
//            observer->notify2bit(angle, _2bitbuffer.data());
//    }
}
///----------------------------------------------------------------------------------------------

void ArkanProcessing::processTargets(unsigned angle, const void *data, size_t size) noexcept
{
    // TODO: в случае передачи готовых отметок с радара сделать преобразование в цели либо передачу
    // в плагин sentry отметок для будущей передачи в трекер (processTargets -> Sentry -> Tracker)
    Q_UNUSED(angle)
    Q_UNUSED(data)
    Q_UNUSED(size)
}

//ArkanProcessing::State ArkanProcessing::groundState()
//{
//    return _groundState;
//}

//float ArkanProcessing::threshold() const noexcept
//{
//    return _threshold;
//}

//void ArkanProcessing::setThreshold(float threshold) noexcept
//{
//    QMutexLocker _locker(&_mutex);

//    if (qFuzzyCompare(_threshold + 1, threshold + 1))
//        return;

//    _threshold = threshold;

//    std::fill(_thresholdLine.begin(), _thresholdLine.end(), _threshold);
//}

//ISpotCombiner *ArkanProcessing::spotCombiner() const
//{
//    return _spotCombiner.get();
//}

//Ground::SectorGround2d *ArkanProcessing::ground() const
//{
//    return _ground2d.get();
//}

//void ArkanProcessing::resetGround()
//{
//    QMutexLocker locker(&_mutex);

//    _ground2d->reset();
//    _groundState = State::Accumulation;
//}

//unsigned ArkanProcessing::rain() const
//{
//    return _rain.rain();
//}

//void ArkanProcessing::setRain(unsigned rain)
//{
//    if (_rain.rain() == rain)
//        return;

//    _rain.setRain(rain);

//    emit rainChanged(rain);
//}

//unsigned ArkanProcessing::sea() const
//{
//    return _sea.sea();
//}

//void ArkanProcessing::setSea(unsigned sea)
//{
//    QMutexLocker locker(&_mutex);

//    if (_sea.sea() == sea)
//        return;

//    _sea.setSea(sea);

//    emit seaChanged(sea);
//}

//unsigned ArkanProcessing::seaDistance() const
//{
//    return _sea.distance();
//}

//void ArkanProcessing::setSeaDistance(unsigned seaDistance)
//{
//    QMutexLocker locker(&_mutex);

//    if (_sea.distance() == seaDistance)
//        return;

//    _sea.setDistance(seaDistance);

//    emit seaDistanceChanged(seaDistance);
//}

//unsigned ArkanProcessing::gain() const
//{
//    return _gain.gain();
//}

//void ArkanProcessing::setGain(unsigned gain)
//{
//    QMutexLocker locker(&_mutex);

//    if (_gain.gain() == gain)
//        return;

//    _gain.setGain(gain);

//    emit gainChanged(gain);
//}

//float ArkanProcessing::antennaHeight() const
//{
//    return _antennaHeight;
//}

//void ArkanProcessing::setAntennaHeight(float height)
//{
//    QMutexLocker locker(&_mutex);

//    if (qFuzzyCompare(_antennaHeight + 1.f, height + 1.f))
//        return;

//    _antennaHeight = height;

//    emit antennaHeightChanged(height);
//}

//void ArkanProcessing::addZone(IMarkFilterZone *zone)
//{
//    // Поскольку нет способа остановить запущенные задачи, связанные с зоной
//    // из пула задач, то нужно отслеживать существование зоны внутри этих задач, которые могли
//    // выполняться на момент удаление зоны
//    const QPointer<IMarkFilterZone> zonePointer(zone);

//    _zoneQueue.start([this, zonePointer]() {
//        const auto zoneMatrix = _zoneFilter.matrix();
//        const auto id = zoneMatrix->addParameters(
//         zonePointer->intensity(), zonePointer->constantThreshold(), zonePointer->size());
//        zoneMatrix->addZone(_ppi->centerCoordinate(), zonePointer->coordinates(), id);
//        zoneMatrix->setZoneActive(id, zonePointer->work());

//        const auto replaceTask = [this, id](ZoneFilterRunnable *runnable) {
//            // Если имеется задача по изменению координат, то пытаемся ее удалить, поскольку
//            // она более не актуальна
//            const auto oldRunnable = _zoneRunnables.value(id, nullptr);
//            if (_zoneQueue.tryTake(oldRunnable))
//                oldRunnable->deleteLater();

//            // Помещаем новую задачу по изменению координат
//            runnable->setAutoDelete(true);
//            _zoneRunnables[id] = runnable;
//            _zoneQueue.start(runnable);
//        };

//        connect(zonePointer, &IMarkFilterZone::intensityChanged, this,
//                [this, zoneMatrix, id](auto intensity) {
//                    QMutexLocker locker(&_mutex);
//                    const auto parameters = zoneMatrix->parameters(id);
//                    parameters->setIntensityMult(intensity);
//                });

//        connect(zonePointer, &IMarkFilterZone::constantThresholdChanged, this,
//                [this, zoneMatrix, id](auto threshold) {
//                    QMutexLocker locker(&_mutex);
//                    const auto parameters = zoneMatrix->parameters(id);
//                    parameters->setConstantThreshold(threshold);
//                });

//        connect(zonePointer, &IMarkFilterZone::sizeChanged, this,
//                [this, zoneMatrix, id](auto size) {
//                    QMutexLocker locker(&_mutex);
//                    const auto parameters = zoneMatrix->parameters(id);
//                    parameters->setAverageLevel(size);
//                });

//        connect(zonePointer, &IMarkFilterZone::workChanged, this,
//                [this, zoneMatrix, id](bool work) {
//                    QMutexLocker locker(&_mutex);
//                    zoneMatrix->setZoneActive(id, work);
//                });

//        connect(
//         zonePointer, &IMarkFilterZone::createdChanged, this,
//         [this, zoneMatrix, id, zonePointer, replaceTask]() {
//             const auto runnable = new ZoneFilterRunnable(
//              [this, zoneMatrix, id, zonePointer, coordinates = zonePointer->coordinates()]() {
//                  if (zonePointer)
//                      zoneMatrix->setZoneCoordinates(_ppi->centerCoordinate(), coordinates, id);
//              });

//             replaceTask(runnable);
//         });

//        connect(zonePointer, &IMarkFilterZone::coordinatesChanged, this,
//                [this, zoneMatrix, zonePointer, id, replaceTask](const auto &coordinates) {
//                    const auto runnable =
//                     new ZoneFilterRunnable([this, zoneMatrix, id, zonePointer, coordinates]() {
//                         if (zonePointer && zonePointer->created())
//                             zoneMatrix->setZoneCoordinates(_ppi->centerCoordinate(), coordinates,
//                                                            id);
//                     });

//                    replaceTask(runnable);
//                });

//        connect(_ppi, &IPPI::centerCoordinateChanged, zonePointer,
//                [this, zoneMatrix, id, zonePointer, replaceTask](const auto &center) {
//                    const auto runnable =
//                     new ZoneFilterRunnable([this, zoneMatrix, id, zonePointer, center,
//                                             coordinates = zonePointer->coordinates()]() {
//                         if (zonePointer && zonePointer->created())
//                             zoneMatrix->setZoneCoordinates(center, coordinates, id);
//                     });

//                    replaceTask(runnable);
//                });

//        connect(_ppi, &IPPI::rangeChanged, zonePointer,
//         [this, id, zonePointer, zoneMatrix, replaceTask](auto range) {
//             const auto runnable = new ZoneFilterRunnable(
//              [this, zoneMatrix, id, zonePointer, coordinates = zonePointer->coordinates()]() {
//                  if (zonePointer && zonePointer->created())
//                      zoneMatrix->setZoneCoordinates(_ppi->centerCoordinate(), coordinates, id);
//              });

//             zoneMatrix->setMaxRange(range * RadarCommon::LINE_SIZE / RadarCommon::EFFECTIVE_SIZE);
//             replaceTask(runnable);
//         });

//        connect(zonePointer, &IMarkFilterZone::destroyed, this, [this, zoneMatrix, id]() {
//            const auto oldRunnable = _zoneRunnables.value(id, nullptr);
//            if (_zoneQueue.tryTake(oldRunnable))
//                delete oldRunnable;

//            _zoneQueue.start([this, zoneMatrix, id]() {
//                zoneMatrix->removeZone(id);
//            });
//            _zoneRunnables.remove(id);
//        });
//    });
//}

//unsigned ArkanProcessing::lineSize() const noexcept
//{
//    return _lineSize;
//}

//unsigned ArkanProcessing::contrast() const
//{
//    return _contrast;
//}

//void ArkanProcessing::setContrast(unsigned contrast)
//{
//    if (_contrast == contrast)
//        return;

//    _contrast = contrast;

//    emit contrastChanged(contrast);
//}

//unsigned ArkanProcessing::expanding() const
//{
//    return _expanding.expanding();
//}

//void ArkanProcessing::setExpanding(unsigned expanding)
//{
//    _expanding.setExpanding(expanding);
//}

//unsigned ArkanProcessing::hpfSlope() const
//{
//    return _rcalc.hpfSlope();
//}

//void ArkanProcessing::setHpfSlope(float hpfSlope)
//{
//    _rcalc.setHpfSlope(hpfSlope);
//}

//unsigned ArkanProcessing::hpfAtt() const
//{
//    return _rcalc.hpfAtt();
//}

//void ArkanProcessing::setHpfAtt(float hpfAtt)
//{
//    _rcalc.setHpfAtt(hpfAtt);
//}

//unsigned int ArkanProcessing::hpfDistance() const
//{
//    return _rcalc.hpfRange();
//}

//void ArkanProcessing::setHpfDistance(unsigned hpfDistance)
//{
//    _rcalc.setHpfRange(hpfDistance);
//}

//Working:done
void ArkanProcessing::convertSrcToFft(const ComplexI32 *in, ComplexF *out, size_t size)
{
    fftw_complex input[size];
    fftw_complex output[size];

    for(int i = 0; i < size; i++)
    {
        input[i][0] = in[i].real();
        input[i][1] = in[i].imag();
    }

    fftw_plan plan = fftw_plan_dft_1d(size, input,
                                      output, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    fftw_cleanup();

    for(int i = 0; i < size; i++)
    {
        out[i] = {(float)output[i][0], (float)output[i][1]};
    }
}

void ArkanProcessing::convertSrcToFft(const ComplexF *in, ComplexF *out, size_t size)
{
    fftw_complex input[size];
    fftw_complex output[size];

    for(int i = 0; i < size; i++)
    {
        input[i][0] = in[i].real();
        input[i][1] = in[i].imag();
    }

    fftw_plan plan = fftw_plan_dft_1d(size, input,
                                      output, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    fftw_cleanup();

    for(int i = 0; i < size; i++)
    {
        out[i] = {(float)output[i][0], (float)output[i][1]};
    }
}

void ArkanProcessing::convertFftToLog(const float *in, float *out, size_t size)
{
    std::transform(in, in + size, out, [](float x) {
        x = log10(x);
        return std::min(x, 255.f);
    });
}

void ArkanProcessing::convertFftToLog(std::vector<float> &data)
{

//    // FIXME: понять какой gain нужен здесь
//    const auto c = _mm_set1_ps(100 * std::log(2.f));
//    for (size_t i = 0; i < data.size(); i += 4)
//    {
//        auto x = _mm_load_ps(&data[i]);
//        x = _mm_add_ps(x, _mm_set1_ps(1));
//        x = log2f4(x);
//        x = _mm_mul_ps(x, c);
//        x = _mm_min_ps(x, _mm_set1_ps(255.f));
//        _mm_store_ps(&data[i], x);
}

//float ArkanProcessing::sbRatio(unsigned angle, const float *spoke)
//{
//    const auto sumS = std::accumulate(spoke, spoke + RadarCommon::LINE_SIZE, 0.f);
//    const auto background = _ground2d->threshold(angle).data();
//    const auto sumB = std::accumulate(background, background + RadarCommon::LINE_SIZE, 0.f);

//    return ((sumB != 0) ? sumS / sumB : std::numeric_limits<float>::infinity());
//}
