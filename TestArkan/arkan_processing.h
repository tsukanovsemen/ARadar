#ifndef ARKANPROCESSING_H
#define ARKANPROCESSING_H

//#include "IMarkFilterZone.h"
//#include "algorithms.h"
//#include "ground2d.h"
//#include "combiner/spotcombiner.h"
//#include "filters.h"
//#include "rcalc_p.h"
//#include "ISentry.h"
//#include "logic_arkan_export.h"
//#include "zone_filter.h"
//#include "zone_filter_runnable.h"

#include <complex>

#include <QVector>
#include <QMutex>
#include <QPointer>
#include <fftw3.h>
#include "tst_testarkan.h"

enum
{
    SRC_SIZE = 8192,
    FFT_SIZE = 4096,
    EFFECTIVE_SIZE = 3500,
    ANGLES = 4096,
    DEFAULT_ROTATE_COUNT = 10
};

class AbstractArkanProcessingObserver;
class IPPI;

class  ArkanProcessing final : public QObject
{
    Q_OBJECT

    friend testArkan;
public:
    using ComplexF = std::complex<float>;
    using ComplexI32 = std::complex<int32_t>;

    /// Статус 2bit сцены
    enum State
    {
        Accumulation, /// Накопление фона и вычисление порогов
        Processing /// Рабочий режим
    };

    explicit ArkanProcessing();

    void addObserver(AbstractArkanProcessingObserver *observer);
    void removeObserver(AbstractArkanProcessingObserver *observer);

    void processStreamData(const std::vector<char> &data);

    /// Преобразование из I32 в FFT
    void processRawCmplxI32(unsigned angle, const void *data, size_t size) noexcept;

    /// Преобразование из Ftl в FFT
    void processCmplxFlt(unsigned angle, const void *data, size_t size) noexcept;

    /// Преобразование из FFT в LOG (получение 8-битки)
    void processCmplxFFT(unsigned angle, const void *data, size_t size) noexcept;

    /// Преобразование в цели
    void processTargets(unsigned angle, const void *data, size_t size) noexcept;

//    State groundState();

//    float threshold() const noexcept;
//    Q_SLOT void setThreshold(float threshold) noexcept;

//    /// Сигнал испускается при изменении accumulation
//    Q_SIGNAL void accumulationPercentChanged(float value);

//    /// Сигнал испускается при отправке 2битки (работы радара)
//    Q_SIGNAL void radarWorked();

    /// Комбайнер для формирования отметок
//    ISpotCombiner *spotCombiner() const;

//    /// Граунд
//    Ground::SectorGround2d *ground() const;

    /// Сброс граунда
//    void resetGround();

//    void addZone(IMarkFilterZone *zone);

    unsigned rain() const;
    void setRain(unsigned rain);
    Q_SIGNAL void rainChanged(unsigned rain);

    unsigned sea() const;
    void setSea(unsigned sea);
    Q_SIGNAL void seaChanged(unsigned sea);

    unsigned seaDistance() const;
    void setSeaDistance(unsigned seaDistance);
    Q_SIGNAL void seaDistanceChanged(unsigned seaDistance);

    unsigned gain() const;
    void setGain(unsigned gain);
    Q_SIGNAL void gainChanged(unsigned gain);

    /// Высота установки радара
    float antennaHeight() const;
    void setAntennaHeight(float height);
    Q_SIGNAL void antennaHeightChanged(float height);

    unsigned lineSize() const noexcept;

    unsigned contrast() const;
    void setContrast(unsigned contrast);
    Q_SIGNAL void contrastChanged(float contrast);

    unsigned expanding() const;
    void setExpanding(unsigned expanding);

    unsigned hpfSlope() const;
    void setHpfSlope(float hpfSlope);

    unsigned hpfAtt() const;
    void setHpfAtt(float hpfAtt);

    unsigned int hpfDistance() const;
    void setHpfDistance(unsigned hpfDistance);

private:
    void convertSrcToFft(const ComplexI32 *in, ComplexF *out, size_t size);
    void convertSrcToFft(const ComplexF *in, ComplexF *out, size_t size);
    void convertFftToLog(const float *in, float *out, size_t size);
    void convertFftToLog(std::vector<float> &data);

    /// Соотношение строки к фону (spoke to background)
   // float sbRatio(unsigned angle, const float *spoke);

private:
    QVector<AbstractArkanProcessingObserver *> _observers;
    //QPointer<ISentry> _sentry { nullptr }; ///< Sentry для отправки отметок с радара
    IPPI *_ppi { nullptr }; ///< PPI радара

    std::vector<ComplexI32> _srcComplexI32;
    std::vector<float> _srcAbsFlt;
    std::vector<ComplexF> _bufferFftComplexFlt;
    std::vector<ComplexF> _fftOutputBuffer;
    std::vector<ComplexF> _fftComplex;
    std::vector<float> _fftAbs;
    std::vector<float> _8bitBuffer;
    std::vector<ComplexF> _srcComplexFlt;
    std::vector<float> _2bitbuffer;
    std::vector<float> _thresholdLine;
    std::vector<float> _logBuffer;

    float _threshold { 0.0f }; ///< Порог
    float _antennaHeight { 0.f }; ///< Высота установки радара
    unsigned _lineSize { 0 }; ///< Размер линии
    unsigned _contrast { 0 };

//   //std::unique_ptr<Ground::SectorGround2d> _ground2d;
//    State _groundState { State::Accumulation };

    //std::unique_ptr<SpotCombiner> _spotCombiner;

    //RAlg::BlackmanWindow _window { SRC_SIZE, true };

    QRecursiveMutex _mutex;

    //RAlg::RainFilter _rain { LEN_4K }; ///< Фильтр дождя
    //RAlg::SeaFilter _sea { LEN_4K }; ///< Фильтр волн
    //RAlg::GainFilter _gain { LEN_4K, 0.f, 100.f, 5.5f }; ///< Gain фильтр
    //RAlg::Expander _expanding { LEN_4K }; ///< Экспандирование
    //ZoneFilter _zoneFilter; ///< Фильтр зон
    //QThreadPool _zoneQueue; ///< Очередь для фильтра зон
    /// Таблица runnable, которые выполняются для каждой зоны в фильтре зон
    //QHash<int, QPointer<ZoneFilterRunnable>> _zoneRunnables;

    //RSDK::Internal::RCalc _rcalc;
};

#endif // ARKANPROCESSING_H
