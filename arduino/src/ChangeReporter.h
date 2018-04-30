#ifndef CHANGEREPORTER_H__
#define CHANGEREPORTER_H__

class Print;
class Sensor;

class ChangeReporter {
public:
    ChangeReporter (Print& inReportStream, Sensor& ioSensor);
    void SetMinimumChange (float inChange);

    void Update ();
private:
    Print& mReportStream;
    Sensor& mSensor;

    float mPrevious;    
    float mMinimumChange;    
};

#endif /* end of include guard: CHANGEREPORTER_H__ */