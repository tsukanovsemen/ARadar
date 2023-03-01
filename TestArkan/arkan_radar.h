#ifndef ARKAN_RADAR_H
#define ARKAN_RADAR_H

#include <memory>
#include <optional>

#include <QMutex>
#include <QByteArray>
#include <QFuture>
#include <QByteArray>

//#include "ISectorModel.h"
#include "IArkanRadar.h"
#include "arkan_processing.h"
//#include "arkan_ppi_updater.h"
//#include "asyncqueue.h"
//#include "arkan_ppi.h"
//#include "rradararkan.h"

class IMap;
class ISentry;
class QSettings;
class QTimerEvent;

class ArkanRadar final : public IArkanRadar
{
    Q_OBJECT
//    M_OVERRIDE_PROP_DEF(unsigned,
//                        attenuationInQuadrature,
//                        attenuationInQuadrature,
//                        setAttenuationInQuadrature,
//                        attenuationInQuadratureChanged,
//                        0)
//    M_OVERRIDE_PROP_DEF(qint16, hpf, hpf, setHpf, hpfChanged, 1)
//    M_OVERRIDE_PROP_DEF(bool, power, power, setPower, powerChanged, false)
//    M_OVERRIDE_PROP_DEF(RotationMode,
//                        rotationMode,
//                        rotationMode,
//                        setRotationMode,
//                        rotationModeChanged,
//                        RotationMode::STREAMING_AND_ROTATION)
//    /// Для работы с эмулятором
//    M_OVERRIDE_PROP_DEF(bool,
//                        emulatorConnected,
//                        emulatorConnected,
//                        setEmulatorConnected,
//                        emulatorConnectedChanged,
//                        false)
public:
    explicit ArkanRadar(IMap *map, ISentry *sentry, QSettings *settings, int id);
    ~ArkanRadar();

private:
    // RSocketObserver_dev interface
    /// Сырые данные с радара
//    void recvStreamData(RSDK::RRadarStreamDataType::Type radarStreamDataType,
//                        const std::optional<uint32_t> &angle,
//                        const std::vector<char> &data) override;

public:
    // AbstractRadar interface
//    void load() override;
//    void save() override;
//    void clearSettings() override;
//    IMarksFilterModel *marksFilterModel() const override;

    /*int id() const override;

    QString address() override;
    void setAddress(QString address) override;

    Mode mode() override;
    void setMode(Mode mode) override;

    State state() override;
    void setState(AbstractRadar::State state) override;

    QString name() override;
    void setName(QString name) override;

    IPPI *ppi() override;

    unsigned rotationSpeed() const override;
    void setRotationSpeed(unsigned rotationSpeed) noexcept override;

    // IArkanRadar interface
    unsigned signal() const override;
    void setSignal(unsigned signal) noexcept override;

    float antennaHeight() const override;
    void setAntennaHeight(float antennaHeight) noexcept override;

    float accumulationPercent() const override;
    void setAccumulationPercent(float value) noexcept override;*/

//    void requestMaxAttenuation() noexcept override;

    /// TODO: сделать после внедрения embedded
//    unsigned attenuation() const override;
//    /// TODO: сделать после внедрения embedded
//    void setAttenuation(unsigned att) noexcept override;

    /// TODO: сделать после внедрения embedded
    //unsigned attenuationIn() const override;
    /// TODO: сделать после внедрения embedded
    //void setAttenuationIn(unsigned attenuationIn) noexcept override;

    /// TODO: сделать после внедрения embedded
  //  unsigned ir() const override;
    /// TODO: сделать после внедрения embedded
    //void setIr(unsigned ir) noexcept override;

//    unsigned gain() const override;
//    void setGain(unsigned gain) noexcept override;

//    unsigned sea() const override;
//    void setSea(unsigned sea) noexcept override;

//    unsigned seaRange_m() const override;
//    void setSeaRange_m(unsigned range) noexcept override;

//    unsigned rain() const override;
//    void setRain(unsigned rain) noexcept override;

//    Combiner combinerType() const override;

//    ISpotCombiner *spotCombiner() const override;

//    ITA *ita() const override;

//    IBlankSectorListModel *blanksectorsmodel() const override;

//    bool adaptiveThreshold() const override;
//    void setAdaptiveThreshold(bool value) noexcept override;

//    float threshold() const override;
//    void setThreshold(float value) noexcept override;

//    unsigned accumulationCycle() const override;
//    void setAccumulationCycle(unsigned value) noexcept override;

//    double falseAlarmProbability() const override;
//    void setFalseAlarmProbability(double value) noexcept override;

//    unsigned recalculationCycle() const override;
//    void setRecalculationCycle(unsigned value) noexcept override;

//    unsigned windowSize() const override;
//    void setWindowSize(unsigned value) noexcept override;

//    RSDK::RRadarStreamDataType::Type dataType() const noexcept override;
//    void setDataType(RSDK::RRadarStreamDataType::Type dataType) noexcept override;

//    uint32_t connectionDelay() const override;
 //   void setConnectionDelay(uint32_t seconds) noexcept override;

//    ISectorModel *sectors() const override;
    //ArkanProcessing *processing() const override;
   // QByteArray send(uint8_t cmd, const QByteArray &data) override;
    //void sendSectors() noexcept override;

    //void connectToServer() override;
    //void disconnectFromServer() override;

//    std::error_code lastError() const override;

//    unsigned maxAttenuation() const override;

private:
    void makeAverageIndicator();
    void makeVisirIndicator();
    void makeSectorView();
//    void processRawData(RSDK::RRadarStreamDataType::Type radarStreamDataType,
//                        const std::optional<uint32_t> &angle,
//                        const std::vector<char> &data);

private:
    int _id { -1 }; ///< id радара
    QString _ipAddress; ///< ip адресс радара
//    AbstractRadar::State _state {
//        AbstractRadar::State::NotAccessedState
//    }; ///< Состояния работы радара
//    Mode _mode { Mode::StopMode }; ///< Режим работы радара
    bool _streaming { false }; ///< Флаг отвечает за пересылку данных с радара
    QString _name; ///< Имя радара для интерфейса пользователя
    unsigned _rotationSpeed { 12 }; ///< Скорость вращения радара
    float _antennaHeight { 0.f }; ///< Высота установки радара
    std::error_code _lastError; ///< Последнее сообщение об ошибке, полученное при отправки
    unsigned _maxAttenuation { 0 }; ///< Максимальный диапозон передатчика

    std::unique_ptr<ArkanProcessing> _arkanProcessing; ///< Класс обработки данных (математика)

//    QMutex _mutex { QMutex::NonRecursive }; ///< Mutex для блокировки гонки данных

//    std::unique_ptr<ArkanPPI> _ppi; ///< Управляющий класс для RadarScene
//    std::unique_ptr<ArkanPPIUpdater> _arkanPPIUpdater; ///< Отрисовка РЛС с Аркана
//    std::unique_ptr<ITA> _ita;
//    std::unique_ptr<ISectorModel> _sectorModel; ///< Модель хранит в себе сектора

    IMap *_map { nullptr }; ///< Карта для ppi
    ISentry *_sentry { nullptr }; ///< Класс передачи отметок от радара
    QSettings *_settings { nullptr }; ///< Настройки для сохранения данных о работе Аркана

#ifdef GUARD
    int _timerId { -1 };
#endif

    float _accumulationPercent { 0.f }; ///< Процент накопления порогов

    bool _adaptiveThreshold { false }; ///< Флаг работы адаптивного порога
    float _threshold { 10.f }; ///< Порог
    unsigned _accumulationCycle { 10 }; ///< Накопление
    unsigned _windowSize { 100 }; ///< Окно усреднения
//    mutable std::unique_ptr<IMarksFilterModel> _marksFilterModel {
//        nullptr
//    }; ///< Модель радарных зон

    /*CustomThreadPool<std::function<void()> &&> _queue { 20 };
    std::unique_ptr<RSDK::RRadarArkan> _radar { nullptr };*/ ///< Управляющий класс работы с радаром
    unsigned _signal { 0 }; ///< Сигнал радара
};

#endif // ARKANCLIENT_H
