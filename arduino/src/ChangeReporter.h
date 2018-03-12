#ifndef CHANGEREPORTER_H__
#define CHANGEREPORTER_H__

class Print;

class ChangeReporter {
public:
    ChangeReporter (Print& inReportStream, const uint8_t* inAddress, uint8_t inPin);
    void SetMinimumChange (float inChange);

    void Update (float inTemperature);
private:
    Print& mReportStream;    
    const uint8_t* mAddress;
    const uint8_t mPin;
    float mPrevious;    
    float mMinimumChange;    
};

#endif /* end of include guard: CHANGEREPORTER_H__ */