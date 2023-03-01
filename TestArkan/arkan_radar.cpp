#include "arkan_radar.h"

//#include "IMap.h"
//#include "ISentry.h"
//#include "algorithms.h"
//#include "arkan_ppi.h"
//#include "arkan_sector_model.h"
//#include "arkanmarksfiltermodel.h"
//#include "baseqt.h"
//#include "ita/arkan_averaging_proxy_observer.h"
//#include "ita/arkan_plot_updater.h"
//#include "ita/arkan_sector_view_updater.h"
//#include "ita/arkan_visir_proxy_observer.h"
//#include "lineitem.h"
//#include "radarscene.h"
//#include "sectoritem.h"
//#include "socket_tcp_slave_pkg.h"

#ifdef USE_RADAR_EMULATOR
#    include "IRadarEmulator.h"
#endif

//#include <rradar_arkan_sender.h>
//#include <rradar_state.h>

#include <cstring>

#include <QEventLoop>
#include <QSettings>
#include <QThread>
#include <QTimerEvent>
#include <QtConcurrent/QtConcurrent>
#include <QtDebug>

namespace {
    static void arkanRegisterMetaTypes()
    {
//        qmlRegisterUncreatableType<IArkanRadar>("Guard.IArkanRadar", 1, 0, "IArkanRadar",
//                                                "IArkanRadar only for enum");
        qRegisterMetaType<IArkanRadar::Combiner>("IArkanRadar::Combiner");
#ifdef RADAR_CONSOLE
        qRegisterMetaType<IRadarEmulator::EmulatorState>("IRadarEmulator::EmulatorState");
#endif
    }
    Q_CONSTRUCTOR_FUNCTION(arkanRegisterMetaTypes)
} // namespace

// Таблица коэффициентов для расчета номера ячейки
// в зависимости от расстояния визира
constexpr float DP_PER_KM_BY_SIGNAL[] = {
    3500.0f / 2.75f, // 0.7857 м
    3500.0f / 3.4f, // 0.9714 м
    3500.0f / 6.7f, // 1.9143 м
    3500.0f / 26.75f, // 7.6429 м
    0,
    3500.0f / 53.5f // 15.2857 м
};

/*!
 * \brief signalToRange
 * \param signal
 * \warning ПРОВЕРИТЬ нужно
 * \return
 */
constexpr double signalToRange(int signal)
{
    switch (signal) {
    case 0:
        return 2750.;
    case 1:
        return 3400.;
    case 2:
        return 6700.;
    case 3:
        return 26750.;
    case 5:
        return 53500.;
    case 4:
    default:
        return 0;
    }
}

const char *ArkanSender_id = "Micran.RSDK.ArkanSender";

ArkanRadar::ArkanRadar(IMap *map, ISentry *sentry, QSettings *settings, int id) :
    _id(id), _map(map), _sentry(sentry), _settings(settings)
{
    // Инициализируем управляющий класс работы с радаром
//    _radar = std::make_unique<RSDK::RRadarArkan>();

//    RSDK::RConnection::connect(_radar.get(), &RSDK::RRadarArkan::stateChangedSignal,
//                               [this](RSDK::RRadarState::Type state) {
//                                   QMetaObject::invokeMethod(this, [this, state]() {
//                                       switch (state) {
//                                       case RSDK::RRadarState::Working:
//                                           break;
//                                       case RSDK::RRadarState::Available:
//                                           setState(State::ReadyState);
//                                           break;
//                                       case RSDK::RRadarState::Connecting:
//                                           setState(State::ConnectingState);
//                                           break;
//                                       case RSDK::RRadarState::NotAccessible:
//                                       default:
//                                           setState(State::NotAccessedState);
//                                           break;
//                                       }
//                                   });
//                               });

//    RSDK::RConnection::connect(_radar.get(), &RSDK::RRadarArkan::errorOccuredSignal,
//                               [this](const std::string &message, const std::error_code &ec) {
//                                   QMetaObject::invokeMethod(this, [this, message, ec]() {
//                                       qWarning() << "Arkan has been returned error"
//                                                  << QString::fromStdString(message);

//                                       _radar->stop();
//                                       _ppi->scene8bit()->clear();
//                                       _ppi->scene2bit()->clear();
//                                       _arkanProcessing->resetGround();
//                                       _lastError = ec;
//                                   });
//                               });

//    // Создаём PPI
//    _ppi = std::make_unique<ArkanPPI>(_id, _map, _settings, FFT_SIZE, EFFECTIVE_SIZE);
//    _ppi->setRange(2750);
//    _ppi->setDraggable(false);

//    // Создаём updater радарной сцены
//    _arkanPPIUpdater = std::make_unique<ArkanPPIUpdater>(_ppi.get());

//    _arkanProcessing = std::make_unique<ArkanProcessing>(_sentry, _ppi.get());
//    // Подписываем updater на класс, отвечающий за обработку данных (8-битка и 2-битка)
//    _arkanProcessing->addObserver(_arkanPPIUpdater.get());

//    _sectorModel = std::make_unique<ArkanSectorModel>(_settings);

//    _ita = std::make_unique<ITA>();
//    makeAverageIndicator();
//    makeVisirIndicator();
//    makeSectorView();

#ifdef GUARD
    // Уведомляем о измении процента накопления данных
    connect(_arkanProcessing.get(), &ArkanProcessing::accumulationPercentChanged, this,
            [this](float value) {
                setState(AbstractRadar::State::AccumulationState);
                setAccumulationPercent(value);
            });
#endif
//    connect(_arkanProcessing.get(), &ArkanProcessing::radarWorked, this, [this]() {
//        //setState(AbstractRadar::State::RunState);
//    });
    // Уведомление о изменении порога
    //connect(this, &IArkanRadar::thresholdChanged, _arkanProcessing.get(),
     //       &ArkanProcessing::setThreshold);

//    connect(this, &IArkanRadar::signalChanged, this, [this](auto signal) {
//        const auto range = signalToRange(signal);

//        _ppi->setRange(range);
 //   });

//    _queue.setHandler([](auto &&func) {
//        func();
//    });

//    _radar->addObserver(this);

#ifdef GUARD
    _radar->setConnectionDelay(1);
#endif

//    _marksFilterModel = std::make_unique<ArkanMarksFilterModel>(_map->map());

//    connect(_marksFilterModel.get(), &ArkanMarksFilterModel::rowsInserted, this,
//            [this](const QModelIndex &, int id) {
//                const auto zone = _marksFilterModel->zone(id);

//                if (!zone)
//                    return;

//                _arkanProcessing->addZone(zone);
//            });
}

ArkanRadar::~ArkanRadar()
{
   /* _radar->stop();

    _arkanProcessing->removeObserver(_arkanPPIUpdater.get())*/;
}

//void ArkanRadar::recvStreamData(RSDK::RRadarStreamDataType::Type radarStreamDataType,
//                                const std::optional<uint32_t> &angle,
//                                const std::vector<char> &data)
//{
//    // FIXME: обдумать способ передачи сырых данных
//    _queue.push([this, radarStreamDataType, angle, data = std::move(data)]() {
//        processRawData(radarStreamDataType, angle, data);
//    });
//}

//void ArkanRadar::processRawData(RSDK::RRadarStreamDataType::Type radarStreamDataType,
//                                const std::optional<uint32_t> &angle,
//                                const std::vector<char> &data)
//{
//    // Необходима блокировка, чтобы не было гонки среди обработки данных
//    QMutexLocker locker(&_mutex);

//    if (!angle.has_value()) {
//        _arkanProcessing->processStreamData(data);
//        return;
//    }

//    // Без обработки
//    if (radarStreamDataType == RSDK::RRadarStreamDataType::RRadarArkan::RawS32) {
//        auto processRawCmplxI32Data = RSDK::RRadarAbstractObserver::parseToComplexS32(data);

//        const auto rawData = reinterpret_cast<void *>(processRawCmplxI32Data.data());

//        _arkanProcessing->processRawCmplxI32(angle.value(), rawData, processRawCmplxI32Data.size());
//    } else if (radarStreamDataType
//               == RSDK::RRadarStreamDataType::RRadarArkan::FftDistHalf) // Без обработки
//    {
//        auto processCmplxFltData = RSDK::RRadarAbstractObserver::parseToComplexFlt(data);

//        const auto rawData = reinterpret_cast<void *>(processCmplxFltData.data());

//        _arkanProcessing->processCmplxFlt(angle.value(), rawData, processCmplxFltData.size());
//    } else if (radarStreamDataType
//               == RSDK::RRadarStreamDataType::RRadarArkan::FftDistHalfMod) { // Быстрое
//                                                                             // преобразование Фурье
//        auto processCmplxFFTData = RSDK::RRadarAbstractObserver::parseToFft(data);

//        const auto rawData = reinterpret_cast<void *>(processCmplxFFTData.data());

//        _arkanProcessing->processCmplxFFT(angle.value(), rawData, processCmplxFFTData.size());
//    } else {
//        qWarning() << "Unknown command for source data from radar";
//    }
//}

//void ArkanRadar::load()
//{
//    _settings->beginGroup("Radars/Arkan/" + QString::number(_id));
//    {
//        setName(_settings->value("name", _name).toString());
//        setAddress(_settings->value("address", _ipAddress).toString());
//        setRotationSpeed(_settings->value("rotationSpeed", _rotationSpeed).toUInt());
//        setThreshold(_settings->value("threshold", _threshold).toFloat());
//        setAccumulationCycle(_settings->value("accumulationCycle", _accumulationCycle).toUInt());
//        setAdaptiveThreshold(_settings->value("adaptiveThreshold", _adaptiveThreshold).toBool());
//        setWindowSize(_settings->value("windowSize", _windowSize).toUInt());
//        setAntennaHeight(_settings->value("antennaHeight", _antennaHeight).toFloat());

//        const auto latitude = _settings->value("latitude").toDouble();
//        const auto longitude = _settings->value("longitude").toDouble();

//        _mutex.lock();
//        _ppi->setCenterCoordinate(QGeoCoordinate(latitude, longitude));
//        _ppi->setAzimuth(_settings->value("azimuth").toDouble());
//        _ppi->scene8bit()->setDisplayed(_settings->value("show8bitScene", true).toBool());
//        _ppi->scene2bit()->setDisplayed(_settings->value("show2bitScene", true).toBool());
//        _mutex.unlock();

//        setGain(_settings->value("gain", 0).toUInt());
//        setSeaRange_m(_settings->value("seaDistance", 0).toUInt());
//        setSea(_settings->value("sea", 0).toUInt());
//        setRain(_settings->value("rain", 0).toUInt());

//        _sectorModel->load();

//        const auto spotCombiner = _arkanProcessing->spotCombiner();

//        spotCombiner->setMaxDist(_settings->value("maxDist", 0).toUInt());
//        spotCombiner->setMaxDist(_settings->value("gapSize", 0).toUInt());
//        spotCombiner->setMaxDist(_settings->value("spread", true).toBool());
//        spotCombiner->setMaxDist(_settings->value("spreadGapSize", 0).toUInt());
//        spotCombiner->setMaxDist(_settings->value("ignoreSmallSpots", true).toBool());
//        spotCombiner->setMaxDist(_settings->value("azimuthGapSize", 0).toUInt());

//        marksFilterModel()->load(_settings);
//    }
//    _settings->endGroup();

//    _radar->setIp(_ipAddress.toStdString());
//}

//void ArkanRadar::save()
//{
//    QMutexLocker locker(&_mutex);

//    _settings->beginGroup("Radars/Arkan/" + QString::number(_id));
//    {
//        _settings->setValue("name", _name);
//        _settings->setValue("address", _ipAddress);
//        _settings->setValue("rotationSpeed", _rotationSpeed);
//        _settings->setValue("threshold", _threshold);
//        _settings->setValue("accumulationCycle", _accumulationCycle);
//        _settings->setValue("adaptiveThreshold", _adaptiveThreshold);
//        _settings->setValue("windowSize", _windowSize);

//        const auto center = _ppi->centerCoordinate();
//        const auto latitude = center.latitude();
//        const auto longitude = center.longitude();

//        _settings->setValue("latitude", latitude);
//        _settings->setValue("longitude", longitude);
//        _settings->setValue("azimuth", _ppi->azimuth());
//        _settings->setValue("antennaHeight", _antennaHeight);
//        _settings->setValue("show8bitScene", _ppi->scene8bit()->displayed());
//        _settings->setValue("show2bitScene", _ppi->scene2bit()->displayed());
//        _settings->setValue("gain", _arkanProcessing->gain());
//        _settings->setValue("seaDistance", _arkanProcessing->seaDistance());
//        _settings->setValue("sea", _arkanProcessing->sea());
//        _settings->setValue("rain", _arkanProcessing->rain());

//        _sectorModel->save();

//        const auto spotCombiner = _arkanProcessing->spotCombiner();

//        _settings->setValue("maxDist", spotCombiner->maxDist());
//        _settings->setValue("gapSize", spotCombiner->gapSize());
//        _settings->setValue("spread", spotCombiner->spread());
//        _settings->setValue("spreadGapSize", spotCombiner->spreadGapSize());
//        _settings->setValue("ignoreSmallSpots", spotCombiner->ignoreSmallSpots());
//        _settings->setValue("azimuthGapSize", spotCombiner->azimuthGapSize());

//        marksFilterModel()->save(_settings);
//    }
//    _settings->endGroup();
//}

//void ArkanRadar::clearSettings()
//{
//    _settings->remove("Radars/Arkan/" + QString::number(_id));
//}

//IMarksFilterModel *ArkanRadar::marksFilterModel() const
//{
//    return _marksFilterModel.get();
//}

//int ArkanRadar::id() const
//{
//    return _id;
//}

//QString ArkanRadar::address()
//{
//    return _ipAddress;
//}

//void ArkanRadar::setAddress(QString address)
//{
//    QMutexLocker locker(&_mutex);

//    M_SET_DEF(_ipAddress, address, addressChanged);

//    _radar->setIp(_ipAddress.toStdString());
//}

//AbstractRadar::Mode ArkanRadar::mode()
//{
//    return _mode;
//}

//void ArkanRadar::setMode(Mode mode)
//{
//    M_SET_DEF(_mode, mode, modeChanged);

//    // Пришла команда на включение
//    if (_mode == Mode::RunMode) {
//        _radar->start(_rotationSpeed, _signal);

//        setState(AbstractRadar::State::StartingState);
//    } else {
//        _radar->stop();

//        QMetaObject::invokeMethod(
//         this,
//         [this] {
//             _ppi->scene8bit()->clear();
//             _ppi->scene2bit()->clear();
//             _arkanProcessing->resetGround();
//         },
//         Qt::QueuedConnection);

//        setState(State::ReadyState);
//    }
//}

//AbstractRadar::State ArkanRadar::state()
//{
//    return _state;
//}

//void ArkanRadar::setState(AbstractRadar::State state)
//{
//    QMutexLocker locker(&_mutex);

//    M_SET_DEF(_state, state, stateChanged);
//}

//QString ArkanRadar::name()
//{
//    return _name;
//}

//void ArkanRadar::setName(QString name)
//{
//    QMutexLocker locker(&_mutex);

//    M_SET_DEF(_name, name, nameChanged);
//}

//IPPI *ArkanRadar::ppi()
//{
//    return _ppi.get();
//}

//unsigned ArkanRadar::rotationSpeed() const
//{
//    return _rotationSpeed;
//}

//void ArkanRadar::setRotationSpeed(unsigned rotationSpeed) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    if ((_state != AbstractRadar::ReadyState) || (_rotationSpeed == rotationSpeed))
//        return;

//    QByteArray data(sizeof(char), (rotationSpeed <= 30) ? static_cast<char>(rotationSpeed) : 0);

//    send(nArcn::eRqst_Mtr_SetMtrSpeed, data);

//    _rotationSpeed = rotationSpeed;

//    M_SET_DEF(_rotationSpeed, rotationSpeed, rotationSpeedChanged);

//    if (_radar->state() == RSDK::RRadarState::Working)
//        _radar->setEngineSpeed(rotationSpeed);
//}

//unsigned ArkanRadar::signal() const
//{
//    return _signal;
//}

//void ArkanRadar::setSignal(unsigned signal) noexcept
//{
//    if (_emulatorConnected || _state == State::NotAccessedState)
//        return;

//    QMutexLocker locker(&_mutex);

//    M_SET_DEF(_signal, signal, signalChanged);

//    _radar->setSignal(signal);
//}

//float ArkanRadar::antennaHeight() const
//{
//    return _antennaHeight;
//}

//void ArkanRadar::setAntennaHeight(float antennaHeight) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    if (qFuzzyCompare(_antennaHeight + 1.f, antennaHeight + 1.f))
//        return;

//    _antennaHeight = antennaHeight;
//    _arkanProcessing->setAntennaHeight(antennaHeight);

//    emit antennaHeightChanged(antennaHeight);
//}

//float ArkanRadar::accumulationPercent() const
//{
//    return _accumulationPercent;
//}

//void ArkanRadar::setAccumulationPercent(float value) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    if (qFuzzyCompare(_accumulationPercent + 1, value + 1))
//        return;

//    _accumulationPercent = value;

//    emit accumulationPercentChanged(_accumulationPercent);
//}

//void ArkanRadar::requestMaxAttenuation() noexcept
//{
//    if (_state != State::NotAccessedState)
//        return;

//    QByteArray baMaxAttenuation = send(nArcn::eRqst_GetAttntrTxMax, {});
//    std::copy(baMaxAttenuation.data(), baMaxAttenuation.data() + baMaxAttenuation.size(),
//              &_maxAttenuation);

//    emit maxAttenuationChanged(_maxAttenuation);
//}

//unsigned ArkanRadar::attenuation() const
//{
//    return 0;
//}

//void ArkanRadar::setAttenuation(unsigned att) noexcept
//{
//    Q_UNUSED(att)
//}

//unsigned ArkanRadar::attenuationIn() const
//{
//    return 0;
//}

//void ArkanRadar::setAttenuationIn(unsigned attenuationIn) noexcept
//{
//    Q_UNUSED(attenuationIn);
//}

//unsigned ArkanRadar::ir() const
//{
//    return 0;
//}

//void ArkanRadar::setIr(unsigned ir) noexcept
//{
//    Q_UNUSED(ir);
//}

//unsigned ArkanRadar::gain() const
//{
//    return _arkanProcessing->gain();
//}

//void ArkanRadar::setGain(unsigned gain) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    _arkanProcessing->setGain(gain);
//}

//unsigned ArkanRadar::sea() const
//{
//    return _arkanProcessing->sea();
//}

//void ArkanRadar::setSea(unsigned sea) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    _arkanProcessing->setSea(sea);
//}

//unsigned ArkanRadar::seaRange_m() const
//{
//    return _arkanProcessing->seaDistance();
//}

//void ArkanRadar::setSeaRange_m(unsigned range) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    _arkanProcessing->setSeaDistance(range);
//}

//unsigned ArkanRadar::rain() const
//{
//    return _arkanProcessing->rain();
//}

//void ArkanRadar::setRain(unsigned rain) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    _arkanProcessing->setRain(rain);
//}

//IArkanRadar::Combiner ArkanRadar::combinerType() const
//{
//    return IArkanRadar::Combiner::SpotCombiner;
//}

//ISpotCombiner *ArkanRadar::spotCombiner() const
//{
//    return _arkanProcessing->spotCombiner();
//}

//ITA *ArkanRadar::ita() const
//{
//    return _ita.get();
//}

//IBlankSectorListModel *ArkanRadar::blanksectorsmodel() const
//{
//    return nullptr;
//}

//bool ArkanRadar::adaptiveThreshold() const
//{
//    return _adaptiveThreshold;
//}

//void ArkanRadar::setAdaptiveThreshold(bool value) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    _arkanProcessing->ground()->setAdaptiveThreshold(value);

//    M_SET_DEF(_adaptiveThreshold, value, adaptiveThresholdChanged);
//}

//float ArkanRadar::threshold() const
//{
//    return _threshold;
//}

//void ArkanRadar::setThreshold(float value) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    if (qFuzzyCompare(_threshold, value))
//        return;

//    _threshold = value;
//    _arkanProcessing->setThreshold(value);

//    emit thresholdChanged(_threshold);
//}

//unsigned ArkanRadar::accumulationCycle() const
//{
//    return _accumulationCycle;
//}

//void ArkanRadar::setAccumulationCycle(unsigned value) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    if (_accumulationCycle == value || value == 0)
//        return;

//    const auto ground = _arkanProcessing->ground();

//    ground->setAccumulationCycle(value);

//    if (value > _accumulationCycle)
//        ground->reset();

//    _accumulationCycle = value;

//    emit accumulationCycleChanged(_accumulationCycle);
//}

//double ArkanRadar::falseAlarmProbability() const
//{
//    return _arkanProcessing->ground()->falseAlarmProbability();
//}

//void ArkanRadar::setFalseAlarmProbability(double value) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    const auto ground = _arkanProcessing->ground();

//    if (qFuzzyCompare(ground->falseAlarmProbability(), value))
//        return;

//    ground->setFalseAlarmProbability(value);

//    emit falseAlarmProbabilityChanged(ground->falseAlarmProbability());
//}

//unsigned ArkanRadar::recalculationCycle() const
//{
//    return _arkanProcessing->ground()->amountToUpdate();
//}

//void ArkanRadar::setRecalculationCycle(unsigned value) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    const auto ground = _arkanProcessing->ground();

//    if (ground->amountToUpdate() == value)
//        return;

//    ground->setAmountToUpdate(value);

//    emit recalculationCycleChanged(value);
//}

//ISectorModel *ArkanRadar::sectors() const
//{
//    return _sectorModel.get();
//}

//ArkanProcessing *ArkanRadar::processing() const
//{
//    return _arkanProcessing.get();
//}

//std::error_code ArkanRadar::lastError() const
//{
//    return _lastError;
//}

//unsigned ArkanRadar::windowSize() const
//{
//    return _windowSize;
//}

//void ArkanRadar::setWindowSize(unsigned value) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    if (_windowSize == value)
//        return;

//    _windowSize = value;
//    _arkanProcessing->ground()->setWindowSize(value);

//    emit windowSizeChanged(value);
//}

//unsigned ArkanRadar::maxAttenuation() const
//{
//    return _maxAttenuation;
//}

//RSDK::RRadarStreamDataType::Type ArkanRadar::dataType() const noexcept
//{
//    return _radar->radarStreamDataType();
//}

//void ArkanRadar::setDataType(RSDK::RRadarStreamDataType::Type dataType) noexcept
//{
//    QMutexLocker locker(&_mutex);

//    if (_radar->radarStreamDataType() == dataType)
//        return;

//    _radar->setRadarStreamDataType(dataType);
//    emit dataTypeChanged(dataType);
//}

//uint32_t ArkanRadar::connectionDelay() const
//{
//    return 0;
//    //    return _radar->connectionDelay();
//}

//void ArkanRadar::setConnectionDelay(uint32_t seconds) noexcept
//{
//    //    QMutexLocker locker(&_mutex);

//    //    M_SET_COMPARE(connectionDelay(), seconds);

//    //    _radar->setConnectionDelay(seconds);
//    //    emit connectionDelayChanged(seconds);
//}

//void ArkanRadar::makeAverageIndicator()
//{
//    const auto plotter = _ita->averageIndicator();
//    const auto proxy = new ArkanAveragingProxyObserver(plotter);
//    const auto plotUpdater = new ArkanItaPlotUpdater(plotter, plotter);

//    _arkanProcessing->addObserver(proxy);
//    proxy->addObserver(plotUpdater);

//    plotter->indexSBox()->setVisible(false);
//    plotter->setWindowTitle(tr("ITA"));
//    const auto azimuthEdit = plotter->azimuthEdit();

//    connect(
//     plotUpdater, &ArkanItaPlotUpdater::angleChanged, azimuthEdit, [azimuthEdit](unsigned pos) {
//         azimuthEdit->setText(QString::number(static_cast<double>(RadarCommon::toDegrees(pos))));
//     });
//    connect(plotter->adjustPButton(), &QPushButton::clicked, plotUpdater, [plotUpdater]() {
//        plotUpdater->setAdjustNext(true);
//    });

//    const auto averagingSBox = plotter->averagingSBox();
//    const auto progressBar = plotter->progressBar();

//    averagingSBox->setValue(static_cast<int>(proxy->averaging()));
//    progressBar->setMaximum(static_cast<int>(proxy->averaging()));
//    connect(averagingSBox, qOverload<int>(&QSpinBox::valueChanged), plotter,
//            [proxy, progressBar](int value) {
//                proxy->setAveraging(value);
//                progressBar->setMaximum(value);
//                progressBar->setVisible(false);
//            });

//    const auto intervalSBox = plotter->intervalSBox();
//    intervalSBox->setValue(static_cast<int>(plotUpdater->interval()));
//    connect(intervalSBox, qOverload<int>(&QSpinBox::valueChanged), plotUpdater,
//            [plotUpdater](int value) {
//                plotUpdater->setUpdateInterval(value);
//            });

//    connect(proxy, &ArkanAveragingProxyObserver::progressChanged, this, [progressBar](int value) {
//        progressBar->setValue(value);
//        progressBar->setVisible(value != progressBar->maximum());
//    });

//    const auto modeCBox = plotter->modeCBox();
//    modeCBox->addItem("SRC CMPLX", ExtendedDataType::SRC_CMPLX);
//    modeCBox->addItem("SRC ABS", ExtendedDataType::SRC_ABS);
//    modeCBox->addItem("FFT CMPLX", ExtendedDataType::FFT_CMPLX);
//    modeCBox->addItem("LOG", ExtendedDataType::LOG);
//    modeCBox->setCurrentText("SRC CMPLX");

//    connect(modeCBox, qOverload<int>(&QComboBox::currentIndexChanged), plotter,
//            [plotUpdater, proxy, modeCBox](int index) {
//                const auto mode = static_cast<ExtendedDataType>(modeCBox->itemData(index).toInt());

//                plotUpdater->setAdjustNext(true);
//                plotUpdater->setDataType(mode);
//                proxy->setDataType(mode);
//            });
//}

//void ArkanRadar::makeVisirIndicator()
//{
//    const auto plotter = _ita->visirIndicator();
//    const auto proxy = new ArkanVisirProxyObserver(plotter);
//    const auto plotUpdater = new ArkanVisirPlotUpdater(plotter, plotter);
//    const auto ppi = _ppi.get();

//    _arkanProcessing->addObserver(proxy);
//    proxy->addObserver(plotUpdater);

//    proxy->setDeltaAngle(10);

//    plotter->indexSBox()->setVisible(false);
//    plotter->setWindowTitle(tr("ITA (Visir)"));

//    const auto setAzimuth = [ppi, proxy](double azimuth) {
//        const auto ppiOffset = ppi->azimuth();
//        proxy->setAngle(RadarCommon::fromDegrees(azimuth - ppiOffset));
//    };
//    setAzimuth(ppi->visirAzimuth());
//    connect(ppi, &IPPI::visirAzimuthChanged, plotter, setAzimuth);

//    const auto setRange = [plotUpdater, plotter, this](double range) {
//        switch (plotUpdater->dataType()) {
//        case ExtendedDataType::SRC_CMPLX:
//        case ExtendedDataType::FFT_CMPLX:
//        case ExtendedDataType::LOG:
//        case ExtendedDataType::SUBTRACT:
//        case ExtendedDataType::THRESHOLD:
//        case ExtendedDataType::BACKGROUND:
//            /*case ExtendedDataType::BOTH:*/ {
//                auto index = qBound(0u, this->signal(), 5u);
//                float dpKm = DP_PER_KM_BY_SIGNAL[index];

//                plotter->setMarkerPos(std::floor(static_cast<double>(dpKm) * range / 1000.));
//                break;
//            }
//        default:
//            plotter->setMarkerPos(0.);
//            break;
//        }
//    };

//    setRange(ppi->visirRange());
//    connect(ppi, &IPPI::visirRangeChanged, plotter, setRange);

//    connect(plotter, &Plotter::markerPosUpdated, ppi, [ppi, plotter, this](double pos) {
//        const auto index = qBound(0u, this->signal(), 5u);
//        const float dpKm = DP_PER_KM_BY_SIGNAL[index];
//        const auto range = std::floor(1000 * pos / static_cast<double>(dpKm));

//        ppi->setVisirCoordinate(
//         ppi->centerCoordinate().atDistanceAndAzimuth(range, ppi->visirAzimuth()));
//        plotter->setMarkerPos(std::floor(pos));
//    });

//    connect(plotter, &Plotter::visibilityChanged, this, [ppi, this](bool visible) {
//        ppi->visirAnchor()->setVisible(visible || _ita->sectorView()->isVisible());
//    });

//    connect(plotter->adjustPButton(), &QPushButton::clicked, plotUpdater, [plotUpdater]() {
//        plotUpdater->setAdjustNext(true);
//    });

//    connect(this, &IArkanRadar::signalChanged, ppi, [ppi, setRange](unsigned) {
//        setRange(ppi->visirRange());
//    });

//    const auto modeCBox = plotter->modeCBox();
//    modeCBox->addItem(QObject::tr("SRC CMPLX"), ExtendedDataType::SRC_CMPLX);
//    modeCBox->addItem(QObject::tr("SRC ABS"), ExtendedDataType::SRC_ABS);
//    modeCBox->addItem(QObject::tr("FFT CMPLX"), ExtendedDataType::FFT_CMPLX);
//    modeCBox->addItem(QObject::tr("LOG"), ExtendedDataType::LOG);
//    modeCBox->addItem(QObject::tr("SUBTRACT"), ExtendedDataType::SUBTRACT);
//    modeCBox->addItem(QObject::tr("THRESHOLD"), ExtendedDataType::THRESHOLD);
//    modeCBox->addItem(QObject::tr("BACKGROUND"), ExtendedDataType::BACKGROUND);
//    // modeCBox->addItem(QObject::tr("BOTH"), ExtendedDataType::BOTH);
//    modeCBox->setCurrentText(QObject::tr("SRC CMPLX"));

//    connect(modeCBox, qOverload<int>(&QComboBox::currentIndexChanged), plotter,
//            [plotUpdater, setRange, modeCBox, this](int index) {
//                plotUpdater->setAdjustNext(true);
//                plotUpdater->setDataType(ExtendedDataType(modeCBox->itemData(index).toInt()));
//                setRange(_ppi->visirRange());
//            });

//    const auto visirDeltaLineLeft = new LineItem(_map->map());
//    visirDeltaLineLeft->setVisible(false);
//    visirDeltaLineLeft->setZ(2);
//    visirDeltaLineLeft->setDashPattern({ 8, 8 });
//    visirDeltaLineLeft->setColor(QColor(Qt::red));
//    visirDeltaLineLeft->setPoint1(_ppi->anchor()->position());

//    const auto visirDeltaLineRight = new LineItem(_map->map());
//    visirDeltaLineRight->setVisible(false);
//    visirDeltaLineRight->setZ(2);
//    visirDeltaLineRight->setDashPattern({ 8, 8 });
//    visirDeltaLineRight->setColor(QColor(Qt::red));
//    visirDeltaLineRight->setPoint1(_ppi->anchor()->position());

//    connect(this, &QObject::destroyed, visirDeltaLineLeft,
//            [visirDeltaLineLeft, visirDeltaLineRight]() {
//                visirDeltaLineLeft->deleteLater();
//                visirDeltaLineRight->deleteLater();
//            });

//    const auto anchor = _ppi->anchor();
//    const auto visirPoint = _ppi->visirPoint();

//    const auto updateX2 = [this, proxy, visirDeltaLineLeft, visirDeltaLineRight]() {
//        const auto anchor = _ppi->anchor();
//        const auto visirPoint = _ppi->visirPoint();
//        const auto deltaY = visirPoint->y() - anchor->y();
//        const auto deltaX = visirPoint->x() - anchor->x();

//        const float visirAngle = std::atan2(deltaY, deltaX);
//        const float angle = RadarCommon::toRad(proxy->deltaAngle());

//        const auto lineLength = std::sqrt(std::pow(deltaX, 2) + std::pow(deltaY, 2));

//        visirDeltaLineLeft->setX2(anchor->x()
//                                  + lineLength * std::cos(static_cast<double>(visirAngle - angle)));
//        visirDeltaLineRight->setX2(
//         anchor->x() + lineLength * std::cos(static_cast<double>(visirAngle + angle)));
//    };

//    const auto updateY2 = [this, proxy, visirDeltaLineLeft, visirDeltaLineRight]() {
//        const auto anchor = _ppi->anchor();
//        const auto visirPoint = _ppi->visirPoint();
//        const auto deltaY = visirPoint->y() - anchor->y();
//        const auto deltaX = visirPoint->x() - anchor->x();

//        const float visirAngle = std::atan2(deltaY, deltaX);
//        const float angle = RadarCommon::toRad(proxy->deltaAngle());

//        const auto lineLength = std::sqrt(std::pow(deltaX, 2) + std::pow(deltaY, 2));

//        visirDeltaLineLeft->setY2(anchor->y()
//                                  + lineLength * std::sin(static_cast<double>(visirAngle - angle)));
//        visirDeltaLineRight->setY2(
//         anchor->y() + lineLength * std::sin(static_cast<double>(visirAngle + angle)));
//    };

//    connect(anchor, &QQuickItem::xChanged, this,
//            [anchor, visirDeltaLineLeft, visirDeltaLineRight, updateX2] {
//                visirDeltaLineLeft->setX1(anchor->x());
//                visirDeltaLineRight->setX1(anchor->x());
//                updateX2();
//            });
//    connect(anchor, &QQuickItem::yChanged, this,
//            [anchor, visirDeltaLineLeft, visirDeltaLineRight, updateY2] {
//                visirDeltaLineLeft->setY1(anchor->y());
//                visirDeltaLineRight->setY1(anchor->y());
//                updateY2();
//            });

//    connect(visirPoint, &QQuickItem::xChanged, this, updateX2);
//    connect(visirPoint, &QQuickItem::yChanged, this, updateY2);

//    connect(plotter->deltaAngleSBox(), qOverload<double>(&QDoubleSpinBox::valueChanged), this,
//            [proxy, updateX2, updateY2](double value) {
//                proxy->setDeltaAngle(RadarCommon::fromDegrees(value));
//                updateX2();
//                updateY2();
//            });
//    connect(plotter, &Plotter::visibilityChanged, this,
//            [visirDeltaLineLeft, visirDeltaLineRight](bool visible) {
//                visirDeltaLineLeft->setVisible(visible);
//                visirDeltaLineRight->setVisible(visible);
//            });

//    updateX2();
//    updateY2();
//}

//void ArkanRadar::makeSectorView()
//{
//    const auto sectorView = _ita->sectorView();
//    const auto svu = new ArkanSectorViewUpdater(sectorView);
//    const auto ppi = _ppi.get();

//    _arkanProcessing->addObserver(svu);

//    const auto updateCoordinate = [svu, this] {
//        const double angle = _ppi->visirAzimuth() - _ppi->azimuth();
//        svu->setPosition(RadarCommon::fromDegrees(angle));
//        svu->setDistance(static_cast<double>(RadarCommon::EFFECTIVE_SIZE) * _ppi->visirRange()
//                         / _ppi->range());
//    };
//    connect(ppi, &IPPI::visirCoordinateChanged, svu, updateCoordinate);
//    updateCoordinate();

//    const auto textBox = sectorView->textEdit();
//    connect(svu, &ArkanSectorViewUpdater::updateStatistics, textBox, &QTextEdit::append);

//    connect(sectorView, &SectorWidget::visibilityChanged, this, [this](bool visible) {
//        _ppi->visirAnchor()->setVisible(visible || _ita->visirIndicator()->isVisible());
//    });

//    const auto enableChBox = sectorView->enableChBox();
//    enableChBox->setChecked(svu->enabled());
//    connect(enableChBox, &QCheckBox::toggled, svu, &ArkanSectorViewUpdater::setEnabled);
//    connect(svu, &ArkanSectorViewUpdater::enabledChanged, enableChBox, &QCheckBox::setChecked);

//    const auto distWindow = sectorView->distWindow();
//    distWindow->setValue(static_cast<int>(svu->distanceWindow()));

//    const auto updateDistanceWindowSuffix = [distWindow, this] {
//        const auto range =
//         _ppi->range() * distWindow->value() / static_cast<double>(RadarCommon::EFFECTIVE_SIZE);
//        distWindow->setSuffix(QString(" [%1 m.]").arg(range, 0, 'f', 1));
//    };

//    connect(distWindow, qOverload<int>(&QSpinBox::valueChanged), svu,
//            &ArkanSectorViewUpdater::setDistanceWindow);

//    connect(svu, &ArkanSectorViewUpdater::distanceWindowChanged, distWindow, &QSpinBox::setValue);
//    connect(svu, &ArkanSectorViewUpdater::distanceWindowChanged, distWindow,
//            updateDistanceWindowSuffix);
//    updateDistanceWindowSuffix();

//    const auto angleWindow = sectorView->angleWindow();
//    angleWindow->setValue(static_cast<int>(svu->positionWindow()));

//    const auto updateAngleWindowSuffix = [angleWindow] {
//        const auto angle = RadarCommon::toDegrees(static_cast<unsigned>(angleWindow->value()));
//        angleWindow->setSuffix(QString(" [%1°]").arg(static_cast<double>(angle), 0, 'f', 2));
//    };
//    connect(angleWindow, qOverload<int>(&QSpinBox::valueChanged), svu,
//            &ArkanSectorViewUpdater::setPositionWindow);
//    connect(svu, &ArkanSectorViewUpdater::positionWindowChanged, angleWindow, &QSpinBox::setValue);
//    connect(svu, &ArkanSectorViewUpdater::positionWindowChanged, angleWindow,
//            updateAngleWindowSuffix);
//    updateAngleWindowSuffix();

//    const auto sector = new SectorItem(_map->map());
//    sector->setBorderWidth(2);
//    sector->setBorderColor(Qt::yellow);

//    const auto anchor = _ppi->anchor();
//    const auto visirAnchor = _ppi->visirAnchor();

//    const auto updateSectorPos = [svu, sector, anchor, visirAnchor, this] {
//        sector->setCenter(anchor->position());
//        const double dist =
//         std::hypot(visirAnchor->x() - anchor->x(), visirAnchor->y() - anchor->y());
//        sector->setRadius(dist);

//        const float width =
//         dist * svu->distanceWindow() * _ppi->range()
//         / (static_cast<double>(RadarCommon::EFFECTIVE_SIZE) * _ppi->visirRange());
//        sector->setRadialWidth(width);
//    };
//    updateSectorPos();

//    connect(anchor, &QQuickItem::xChanged, sector, updateSectorPos);
//    connect(anchor, &QQuickItem::yChanged, sector, updateSectorPos);
//    connect(visirAnchor, &QQuickItem::xChanged, sector, updateSectorPos);
//    connect(visirAnchor, &QQuickItem::yChanged, sector, updateSectorPos);
//    connect(svu, &ArkanSectorViewUpdater::distanceWindowChanged, sector, updateSectorPos);
//    connect(svu, &ArkanSectorViewUpdater::enabledChanged, sector, &SectorItem::setVisible);

//    sector->setVisible(enableChBox->isChecked());
//    connect(sectorView, &SectorWidget::visibilityChanged, sector,
//            [sector, enableChBox](bool visible) {
//                sector->setVisible(visible ? enableChBox->isChecked() : false);
//            });

//    sector->setAlpha(_ppi->visirAzimuth());
//    connect(ppi, &IPPI::visirAzimuthChanged, sector, &SectorItem::setAlpha);

//    sector->setTheta(RadarCommon::toDegrees(svu->positionWindow()));
//    connect(svu, &ArkanSectorViewUpdater::positionWindowChanged, sector, [=](unsigned angle) {
//        sector->setTheta(RadarCommon::toDegrees(angle));
//    });
//}

//QByteArray ArkanRadar::send(uint8_t cmd, const QByteArray &data)
//{
//    const auto sender = _radar->sender(ArkanSender_id);
//    const std::vector<char> requestData(data.begin(), data.end());
//    const auto response = sender->send(cmd, requestData);
//    QByteArray result(response.data.data(), response.data.size());
//    _lastError = std::move(response.error);

//    return result;
//}

//void ArkanRadar::sendSectors() noexcept
//{
//    nArcn::sSectors sectorsStruct;

//    sectorsStruct.enable.U32 = 0;

//    for (int i = 0; i < _sectorModel->rowCount(); ++i) {
//        const auto sector = _sectorModel->data(_sectorModel->index(i), ISectorModel::SectorRole)
//                             .value<ArkanSector *>();

//        if (sector->enable())
//            sectorsStruct.enable.U32 |= (1 << i);
//        else
//            sectorsStruct.enable.U32 &= (0xffff - static_cast<int>(pow(2, i)));

//        sectorsStruct.s[i] = sector->sectorStruct();
//    }

//    QByteArray byteArraySectors(sizeof(sectorsStruct), 0x00);
//    std::memcpy(byteArraySectors.data(), &sectorsStruct, sizeof(sectorsStruct));

//    send(nArcn::eRqst_SectorsLoad, byteArraySectors);
//}

//void ArkanRadar::connectToServer()
//{
//    // FIXME: during merge
//    //    if (_socket->state() == RSDK::MProtocol::eRSocketState::disconnected) {
//    //        QMutexLocker locker(&_mutex);
//    //        _socket->connect();
//    //    }
//}

//void ArkanRadar::disconnectFromServer()
//{
//    // FIXME: during merge
//    //    if (_socket->state() != RSDK::MProtocol::eRSocketState::disconnected)
//    //        _socket->disconnect();
//}
