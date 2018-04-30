#ifndef SENSOR_H__
#define SENSOR_H__

class Sensor {
public:
    explicit Sensor ();
    virtual ~Sensor () = default;

    void SetName (const char* inName);
    const char* GetName () const;

    virtual bool GetMeasurement (float& outValue) = 0;
private:
    char mName [32];
};

#endif /* end of include guard: SENSOR_H__ */