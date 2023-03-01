#ifndef IARKANRADAR_H
#define IARKANRADAR_H

//#include <abstract_radar.h>
//#include "ISectorModel.h"
//#include "arkan_export.h"
//#include "combiner/ISpotCombiner.h"
//#include "ita.h"
//#include "baseqt.h"
//#include <rradar_abstract_observer.h>
//#include <rradararkan.h>
//#include "socket_tcp_slave_pkg.h"
#include "arkan_processing.h"
#include <QObject>

//namespace RadarType {
//    constexpr Type ARKAN = Type(0x41524b414e);
//}

class  IArkanRadar:public QObject
{
    Q_OBJECT

public:
    enum Combiner
    {
        SpotCombiner
    };
    Q_ENUM(Combiner)

    enum RotationMode
    {
        STREAMING_AND_ROTATION,
        STREAMING_ONLY,
        ROTATION_ONLY
    };
    Q_ENUM(RotationMode)

    virtual ~IArkanRadar() = default;

//    virtual ITA *ita() const override = 0;

//    virtual IBlankSectorListModel *blanksectorsmodel() const override = 0;

//    virtual ArkanProcessing *processing() const = 0;
//    virtual QByteArray send(uint8_t cmd, const QByteArray &data) = 0;
//    virtual void sendSectors() noexcept = 0;

//    virtual void connectToServer() = 0;
//    virtual void disconnectFromServer() = 0;

//    virtual void requestMaxAttenuation() noexcept = 0;

//    virtual uint32_t connectionDelay() const = 0;
//    virtual void setConnectionDelay(uint32_t seconds) noexcept = 0;
  //  Q_SIGNAL void connectionDelayChanged(uint32_t seconds);

//    virtual RSDK::RRadarStreamDataType::Type dataType() const noexcept = 0;
//    virtual void setDataType(RSDK::RRadarStreamDataType::Type dataType) noexcept = 0;
//    Q_SIGNAL void dataTypeChanged(RSDK::RRadarStreamDataType::Type dataType);

//    M_DECLARE_PVIRT_PROP_CONSTANT(std::error_code, lastError, lastError)
    Q_SIGNAL void errorOccured(const std::error_code &ec);

//    M_DECLARE_PVIRT_PROP_CONSTANT(unsigned, maxAttenuation, maxAttenuation)
    Q_SIGNAL void maxAttenuationChanged(unsigned maxAttenuation);

private:
//    M_DECLARE_PVIRT_PROP(unsigned, signal, signal, setSignal, signalChanged)
//    M_DECLARE_PVIRT_PROP(float,
//                         accumulationPercent,
//                         accumulationPercent,
//                         setAccumulationPercent,
//                         accumulationPercentChanged)
//    M_DECLARE_PVIRT_PROP(unsigned, attenuation, attenuation, setAttenuation, attenuationChanged)
//    M_DECLARE_PVIRT_PROP(
//     unsigned, attenuationIn, attenuationIn, setAttenuationIn, attenuationInChanged)
//    M_DECLARE_PVIRT_PROP(unsigned,
//                         attenuationInQuadrature,
//                         attenuationInQuadrature,
//                         setAttenuationInQuadrature,
//                         attenuationInQuadratureChanged)
//    M_DECLARE_PVIRT_PROP(unsigned, ir, ir, setIr, irChanged)
//    M_DECLARE_PVIRT_PROP(unsigned, gain, gain, setGain, gainChanged)
//    M_DECLARE_PVIRT_PROP(unsigned, sea, sea, setSea, seaChanged)
//    M_DECLARE_PVIRT_PROP(unsigned, seaRange_m, seaRange_m, setSeaRange_m, seaRange_mChanged)
//    M_DECLARE_PVIRT_PROP(unsigned, rain, rain, setRain, rainChanged)
//    M_DECLARE_PVIRT_PROP(
//     float, antennaHeight, antennaHeight, setAntennaHeight, antennaHeightChanged)
//    M_DECLARE_PVIRT_PROP_CONSTANT(IArkanRadar::Combiner, combinerType, combinerType)
//    M_DECLARE_PVIRT_PROP_CONSTANT(ISpotCombiner *, spotCombiner, spotCombiner)
//    M_DECLARE_PVIRT_PROP(
//     bool, adaptiveThreshold, adaptiveThreshold, setAdaptiveThreshold, adaptiveThresholdChanged)
//    M_DECLARE_PVIRT_PROP(short, hpf, hpf, setHpf, hpfChanged)
//    M_DECLARE_PVIRT_PROP(bool, power, power, setPower, powerChanged)
//    M_DECLARE_PVIRT_PROP(float, threshold, threshold, setThreshold, thresholdChanged)
//    M_DECLARE_PVIRT_PROP(
//     unsigned, accumulationCycle, accumulationCycle, setAccumulationCycle, accumulationCycleChanged)
//    M_DECLARE_PVIRT_PROP(double,
//                         falseAlarmProbability,
//                         falseAlarmProbability,
//                         setFalseAlarmProbability,
//                         falseAlarmProbabilityChanged)
//    M_DECLARE_PVIRT_PROP(unsigned,
//                         recalculationCycle,
//                         recalculationCycle,
//                         setRecalculationCycle,
//                         recalculationCycleChanged)
//    M_DECLARE_PVIRT_PROP(
//     unsigned, rotationSpeed, rotationSpeed, setRotationSpeed, rotationSpeedChanged)
//    M_DECLARE_PVIRT_PROP(unsigned, windowSize, windowSize, setWindowSize, windowSizeChanged)
//    M_DECLARE_PVIRT_PROP_CONSTANT(ISectorModel *, sectors, sectors)
//    M_DECLARE_PVIRT_PROP(
//     RotationMode, rotationMode, rotationMode, setRotationMode, rotationModeChanged)
//    M_DECLARE_PVIRT_PROP(
//     bool, emulatorConnected, emulatorConnected, setEmulatorConnected, emulatorConnectedChanged)
};

#endif // IARKANRADAR_H
