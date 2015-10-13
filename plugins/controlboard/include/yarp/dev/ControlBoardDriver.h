/*
 * Copyright (C) 2007-2013 Istituto Italiano di Tecnologia ADVR & iCub Facility
 * Authors: Enrico Mingo, Alessio Rocchi, Mirko Ferrati, Silvio Traversaro and Alessandro Settimi
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef GAZEBOYARP_CONTROLBOARDDRIVER_HH
#define GAZEBOYARP_CONTROLBOARDDRIVER_HH

#include <yarp/os/Property.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IOpenLoopControl.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IControlMode2.h>
#include <yarp/dev/IInteractionMode.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

extern const double RobotPositionTolerance;

namespace yarp {
    namespace dev {
        class GazeboYarpControlBoardDriver;
    }
}

namespace gazebo {
    namespace common {
        class UpdateInfo;
        class Time;
        class PID;
    }

    namespace physics {
        class Model;
        class Joint;
        typedef boost::shared_ptr<Joint> JointPtr;
    }

    namespace event {
        class Connection;
        typedef boost::shared_ptr<Connection> ConnectionPtr;
    }

    namespace transport {
        class Node;
        class Publisher;

        typedef boost::shared_ptr<Node> NodePtr;
        typedef boost::shared_ptr<Publisher> PublisherPtr;
    }

    namespace msgs {
        class JointCmd;
    }
}

class yarp::dev::GazeboYarpControlBoardDriver:
    public DeviceDriver,
    public IPositionControl2,
    public IVelocityControl2,
    public IAmplifierControl,
    public IEncodersTimed,
		public IMotorEncoders,
    public IControlCalibration2,
    public IControlLimits2,
    public IInteractionMode,
    public DeviceResponder,
    public IControlMode2,
    public ITorqueControl,
    public IPositionDirect,
    public IImpedanceControl,
    public IOpenLoopControl,
    public IPidControl
{
public:

    GazeboYarpControlBoardDriver();
    virtual ~GazeboYarpControlBoardDriver();

    /**
     * Gazebo stuff
     */
    bool gazebo_init();
    void onUpdate(const gazebo::common::UpdateInfo&);

    /**
     * Yarp interfaces start here
     */

    //DEVICE DRIVER
    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    //ENCODERS
    virtual bool getEncoder(int j, double* v); //WORKS
    virtual bool getEncoders(double* encs); //WORKS
    virtual bool resetEncoder(int j); //WORKS
    virtual bool resetEncoders(); //WORKS
    virtual bool setEncoder(int j, double val); //WORKS
    virtual bool setEncoders(const double* vals); //WORKS

    virtual bool getEncoderSpeed(int j, double* sp); //NOT TESTED
    virtual bool getEncoderSpeeds(double* spds); //NOT TESTED

    virtual bool getEncoderAcceleration(int j, double* spds); //NOT IMPLEMENTED
    virtual bool getEncoderAccelerations(double* accs); //NOT IMPLEMENTED

    // ENCODERS TIMED
    virtual bool getEncodersTimed(double* encs, double* time);
    virtual bool getEncoderTimed(int j, double* encs, double* time);

    //MOTOR ENCODERS
    virtual bool getMotorEncoder(int m, double *v);  //NOT TESTED
    virtual bool getMotorEncoders(double *encs);  //NOT TESTED
    virtual bool resetMotorEncoder (int m);  //NOT TESTED
    virtual bool resetMotorEncoders ();  //NOT TESTED
    virtual bool setMotorEncoder (int m, const double val);  //NOT TESTED
    virtual bool setMotorEncoders (const double *vals);  //NOT TESTED

    virtual bool getMotorEncoderSpeed (int m, double *sp);  //NOT TESTED
    virtual bool getMotorEncoderSpeeds (double *spds);  //NOT TESTED

    virtual bool getMotorEncoderAcceleration (int m, double *acc);  //NOT IMPLEMENTED
    virtual bool getMotorEncoderAccelerations (double *accs);  //NOT IMPLEMENTED

    virtual bool getNumberOfMotorEncoders (int *num);  //NOT TESTED

    virtual bool getMotorEncoderCountsPerRevolution (int m, double *cpr);  //NOT IMPLEMENTED
    virtual bool setMotorEncoderCountsPerRevolution (int m, const double cpr);  //NOT IMPLEMENTED

    virtual bool getMotorEncoderTimed (int m, double *encs, double *time); //WORKS
    virtual bool getMotorEncodersTimed (double *encs, double *time); //WORKS

    //POSITION CONTROL
    virtual bool stop(int j); //WORKS
    virtual bool stop(); //WORKS
    virtual bool positionMove(int j, double ref); //WORKS
    virtual bool getAxes(int* ax); // WORKS
    virtual bool positionMove(const double* refs); //WORKS
    /// @arg sp [deg/sec]
    virtual bool setRefSpeed(int j, double sp); //WORKS
    virtual bool getRefSpeed(int j, double* ref); //WORKS
    virtual bool getRefSpeeds(double* spds); //WORKS

    virtual bool relativeMove(int j, double delta); //NOT TESTED
    virtual bool relativeMove(const double* deltas); //NOT TESTED
    virtual bool checkMotionDone(int j, bool* flag); //NOT TESTED
    virtual bool checkMotionDone(bool* flag); //NOT TESTED
    virtual bool setPositionMode(); //NOT TESTED

    // Position Control 2
    virtual bool positionMove(const int n_joint, const int* joints, const double* refs);
    virtual bool relativeMove(const int n_joint, const int* joints, const double* deltas);
    virtual bool checkMotionDone(const int n_joint, const int* joints, bool* flags);
    virtual bool setRefSpeeds(const int n_joint, const int* joints, const double* spds);
    virtual bool setRefAccelerations(const int n_joint, const int* joints, const double* accs);
    virtual bool getRefSpeeds(const int n_joint, const int *joints, double *spds);
    virtual bool getRefAccelerations(const int n_joint, const int *joints, double *accs);
    virtual bool stop(const int n_joint, const int *joints);

    /// @arg spds [deg/sec]
    virtual bool setRefSpeeds(const double *spds); //NOT TESTED

    virtual bool setRefAcceleration(int j, double acc); //NOT IMPLEMENTED
    virtual bool setRefAccelerations(const double *accs); //NOT IMPLEMENTED
    virtual bool getRefAcceleration(int j, double *acc); //NOT IMPLEMENTED
    virtual bool getRefAccelerations(double *accs); //NOT IMPLEMENTED

    //VELOCITY CONTROL 2
    virtual bool setVelocityMode(); //NOT TESTED
    virtual bool velocityMove(int j, double sp); //NOT TESTED
    virtual bool velocityMove(const double *sp); //NOT TESTED
    virtual bool velocityMove(const int n_joint, const int *joints, const double *spds);

    virtual bool setVelPid(int j, const yarp::dev::Pid &pid);
    virtual bool setVelPids(const yarp::dev::Pid *pids);
    virtual bool getVelPid(int j, yarp::dev::Pid *pid);
    virtual bool getVelPids(yarp::dev::Pid *pids);

    //CONTROL MODE
    virtual bool setPositionMode(int j); //WORKS
    virtual bool setVelocityMode(int j); //WORKS
    virtual bool getControlMode(int j, int *mode); //WORKS

    virtual bool setTorqueMode(int j); //NOT TESTED
    virtual bool getControlModes(int *modes); //NOT TESTED

    virtual bool setImpedancePositionMode(int j);//NOT IMPLEMENTED
    virtual bool setImpedanceVelocityMode(int j); //NOT IMPLEMENTED
    virtual bool setOpenLoopMode(int j); //NOT IMPLEMENTED

    // CONTROL MODE 2
    virtual bool getControlModes(const int n_joint, const int *joints, int *modes);
    virtual bool setControlMode(const int j, const int mode);
    virtual bool setControlModes(const int n_joint, const int *joints, int *modes);
    virtual bool setControlModes(int *modes);

    //TORQUE CONTROL
    virtual bool setRefTorque(int j, double t); //NOT TESTED
    virtual bool setRefTorques(const double *t); //NOT TESTED
    virtual bool setTorqueMode(); //NOT TESTED
    virtual bool getRefTorque(int j, double *t);    //NOT TESTED
    virtual bool getRefTorques(double *t);//NOT TESTED
    virtual bool getTorque(int j, double *t); //NOT TESTED
    virtual bool getTorques(double *t); //NOT TESTED

    virtual bool getBemfParam(int j, double *bemf); //NOT IMPLEMENTED
    virtual bool setBemfParam(int j, double bemf); //NOT IMPLEMENTED
    virtual bool setTorquePid(int j, const Pid &pid); //NOT IMPLEMENTED
    virtual bool getTorqueRange(int j, double *min, double *max); //NOT IMPLEMENTED
    virtual bool getTorqueRanges(double *min, double *max); //NOT IMPLEMENTED
    virtual bool setTorquePids(const Pid *pids); //NOT IMPLEMENTED
    virtual bool setTorqueErrorLimit(int j, double limit); //NOT IMPLEMENTED
    virtual bool setTorqueErrorLimits(const double *limits); //NOT IMPLEMENTED
    virtual bool getTorqueError(int j, double *err); //NOT IMPLEMENTED
    virtual bool getTorqueErrors(double *errs); //NOT IMPLEMENTED
    virtual bool getTorquePidOutput(int j, double *out); //NOT IMPLEMENTED
    virtual bool getTorquePidOutputs(double *outs); //NOT IMPLEMENTED
    virtual bool getTorquePid(int j, Pid *pid); //NOT IMPLEMENTED
    virtual bool getTorquePids(Pid *pids); //NOT IMPLEMENTED
    virtual bool getTorqueErrorLimit(int j, double *limit); //NOT IMPLEMENTED
    virtual bool getTorqueErrorLimits(double *limits); //NOT IMPLEMENTED
    virtual bool resetTorquePid(int j); //NOT IMPLEMENTED
    virtual bool disableTorquePid(int j); //NOT IMPLEMENTED
    virtual bool enableTorquePid(int j); //NOT IMPLEMENTED
    virtual bool setTorqueOffset(int j, double v); //NOT IMPLEMENTED

    //IMPEDANCE CTRL
    virtual bool getImpedance(int j, double *stiffness, double *damping); // [Nm/deg] & [Nm*sec/deg]
    virtual bool setImpedance(int j, double stiffness, double damping); // [Nm/deg] & [Nm*sec/deg]
    virtual bool setImpedanceOffset(int j, double offset);
    virtual bool getImpedanceOffset(int j, double* offset);
    virtual bool getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp);

    //IOpenLoopControl interface methods
    virtual bool setRefOutput(int j, double v);
    virtual bool setRefOutputs(const double *v);
    virtual bool getRefOutput(int j, double *v);
    virtual bool getRefOutputs(double *v);
    virtual bool getOutput(int j, double *v);
    virtual bool getOutputs(double *v);
    virtual bool setOpenLoopMode();

    /*
     * IPidControl Interface methods
     */
    virtual bool setPid (int j, const Pid &pid);
    virtual bool setPids (const Pid *pids);
    virtual bool setReference (int j, double ref);
    virtual bool setReferences (const double *refs);
    virtual bool setErrorLimit (int j, double limit);
    virtual bool setErrorLimits (const double *limits);
    virtual bool getError (int j, double *err);
    virtual bool getErrors (double *errs);
    virtual bool getPid (int j, Pid *pid);
    virtual bool getPids (Pid *pids);
    virtual bool getReference (int j, double *ref);
    virtual bool getReferences (double *refs);
    virtual bool getErrorLimit (int j, double *limit);
    virtual bool getErrorLimits (double *limits);
    virtual bool resetPid (int j);
    virtual bool disablePid (int j);
    virtual bool enablePid (int j);
    virtual bool setOffset (int j, double v);

    /*
     * Probably useless stuff here
     */
    //AMPLIFIER CONTROL (inside comanOthers.cpp)
    virtual bool enableAmp(int j); //NOT IMPLEMENTED
    virtual bool disableAmp(int j); //NOT IMPLEMENTED
    virtual bool getCurrent(int j, double *val); //NOT IMPLEMENTED
    virtual bool getCurrents(double *vals); //NOT IMPLEMENTED
    virtual bool setMaxCurrent(int j, double v); //NOT IMPLEMENTED
    virtual bool getMaxCurrent(int j, double *v);  //NOT IMPLEMENTED
    virtual bool getAmpStatus(int *st); //NOT IMPLEMENTED
    virtual bool getAmpStatus(int k, int *v); //NOT IMPLEMENTED

    //CONTROL CALIBRATION (inside comanOthers.cpp)
    virtual bool calibrate2(int j, unsigned int iv, double v1, double v2, double v3); //NOT IMPLEMENTED
    virtual bool done(int j); // NOT IMPLEMENTED

    // CONTROL LIMITS2 (inside comanOthers.cpp)
    bool getLimits(int axis, double *min, double *max); //WORKS
    bool setLimits(int axis, double min, double max); //WORKS
    bool getVelLimits(int axis, double *min, double *max); //NOT IMPLEMENTED
    bool setVelLimits(int axis, double min, double max); //NOT IMPLEMENTED
    /*
     * End of useless stuff
     */

    // IPOSITION DIRECT
    bool setPositionDirectMode();
    bool setPosition(int j, double ref);
    bool setPositions(const int n_joint, const int *joints, double *refs);
    bool setPositions(const double *refs);

    // INTERACTION MODE interface
    bool getInteractionMode(int axis, yarp::dev::InteractionModeEnum* mode);
    bool getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes);
    bool getInteractionModes(yarp::dev::InteractionModeEnum* modes);
    bool setInteractionMode(int axis, yarp::dev::InteractionModeEnum mode);
    bool setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes);
    bool setInteractionModes(yarp::dev::InteractionModeEnum* modes);

private:

    /* PID structures */

    /**
     * Internal PID gains structure used in
     * GazeboYarpPlugins.
     * The gains are stored in radians based units
     * for joint whose configuration space is given
     * by an anguar quantity, and meter based units
     * for joints whose configuratios space is given
     * by a linear quantity.
     *
     * \note The gains for angular quantities in YARP
     *       are instead usually expressed in degrees-
     *       based quantity, so an appropriate conversion
     *       is used in the setPids/getPids functions.
     */
    struct PID {
        double p;
        double i;
        double d;
        double maxInt;
        double maxOut;
    };

    enum PIDFeedbackTerm {
        PIDFeedbackTermProportionalTerm = 1,
        PIDFeedbackTermIntegrativeTerm = 1 << 1,
        PIDFeedbackTermDerivativeTerm = 1 << 2,
        PIDFeedbackTermAllTerms = PIDFeedbackTermProportionalTerm | PIDFeedbackTermIntegrativeTerm | PIDFeedbackTermDerivativeTerm
    };

    struct Range {
        Range() : min(0), max(0){}
        double min;
        double max;
    };

    std::string deviceName;
    gazebo::physics::Model* m_robot;
    gazebo::event::ConnectionPtr m_updateConnection;



    yarp::os::Property m_pluginParameters; /**< Contains the parameters of the device contained in the yarpConfigurationFile .ini file */

    unsigned int m_robotRefreshPeriod; //ms
    unsigned int m_numberOfJoints; /**< number of joints controlled by the control board */
    std::vector<Range> m_jointLimits;

    // Motor data
    unsigned int m_numberOfMotorJoints; /**< number of motors controlled by the control board */

    /**
     * The zero position is the position of the GAZEBO joint that will be read as the starting one
     * i.e. getEncoder(j)=m_zeroPosition+gazebo.getEncoder(j);
     */
    yarp::sig::Vector m_zeroPosition;
    yarp::sig::Vector m_motor_zeroPosition;

    yarp::sig::Vector m_positions; /**< joint positions [Degrees]*/
    yarp::sig::Vector m_velocities; /**< joint velocities [Degrees/Seconds] */
    yarp::sig::Vector m_torques; /**< joint torques [Netwon Meters] */
    
    // Motor
    yarp::sig::Vector m_motor_positions; /**< motor positions [Degrees] */
    yarp::sig::Vector m_motor_velocities; /**< motor velocities [Degrees/Seconds] */
    yarp::sig::Vector m_motor_torques; /**< motor torques [Netwon Meters] */

    yarp::os::Stamp m_lastTimestamp; /**< timestamp, updated with simulation time at each onUpdate call */

    yarp::sig::Vector amp;

    //Desired Control variables
    yarp::sig::Vector m_referencePositions; /**< desired reference positions.
                                                 Depending on the position mode,
                                                 they can be set directly or indirectly
                                                 through the trajectory generator.
                                                 [Degrees] */

    yarp::sig::Vector m_referenceTorques; /**< desired reference torques for torque control mode [NetwonMeters] */
    yarp::sig::Vector m_referenceVelocities; /**< desired reference velocities for velocity control mode [Degrees/Seconds] */

    //trajectory generator
    yarp::sig::Vector m_trajectoryGenerationReferencePosition; /**< reference position for trajectory generation in position mode [Degrees] */
    yarp::sig::Vector m_trajectoryGenerationReferenceSpeed; /**< reference speed for trajectory generation in position mode [Degrees/Seconds]*/
    yarp::sig::Vector m_trajectoryGenerationReferenceAcceleraton; /**< reference acceleration for trajectory generation in position mode. Currently NOT USED in trajectory generation! [Degrees/Seconds^2] */
    
    std::vector<std::string> m_jointNames;
    std::vector<gazebo::physics::JointPtr> m_jointPointers; /* pointers for each joint, avoiding several calls to getJoint(joint_name) */
    gazebo::transport::NodePtr m_gazeboNode;
    gazebo::transport::PublisherPtr m_jointCommandPublisher;
    std::vector<GazeboYarpControlBoardDriver::PID> m_positionPIDs;
    std::vector<GazeboYarpControlBoardDriver::PID> m_velocityPIDs;
    std::vector<GazeboYarpControlBoardDriver::PID> m_impedancePosPDs;
    
    std::vector<gazebo::common::PID> m_positionGazeboPIDs;
    std::vector<gazebo::common::PID> m_velocityGazeboPIDs;

    //Motor
    std::vector<std::string> m_motorJointNames;
    std::vector<int> joint_motor_map; // maps joints with motors, contains the corresponding motor joint index
    std::vector<gazebo::physics::JointPtr> m_motorJointPointers; /* pointers for each joint, avoiding several calls to getJoint(joint_name) */

    yarp::sig::Vector m_torqueOffsett;
    yarp::sig::Vector m_minStiffness;
    yarp::sig::Vector m_minDamping;
    yarp::sig::Vector m_maxStiffness;
    yarp::sig::Vector m_maxDamping;

    bool* m_isMotionDone;
    int * m_controlMode;
    int * m_interactionMode;

    bool started;
    int m_clock;
    int _T_controller;

    /**
     * Private methods
     */
    void setMinMaxPos();  //NOT TESTED
    bool setJointNames();  //WORKS
    void setPIDsForGroup(std::string, std::vector<GazeboYarpControlBoardDriver::PID>&, enum PIDFeedbackTerm pidTerms);
    void setMinMaxImpedance();
    void setPIDs(); //WORKS
    void setGazeboPIDs();
    bool sendPositionsToGazebo(yarp::sig::Vector& refs, gazebo::common::Time stepTime);
    bool sendPositionToGazebo(int j,double ref, gazebo::common::Time stepTime);
    bool sendVelocitiesToGazebo(yarp::sig::Vector& refs, gazebo::common::Time stepTime); //NOT TESTED
    bool sendVelocityToGazebo(int j, double ref, gazebo::common::Time stepTime); //NOT TESTED
    bool sendTorquesToGazebo(yarp::sig::Vector& refs, gazebo::common::Time stepTime); //NOT TESTED
    bool sendTorqueToGazebo(const int j,const double ref, gazebo::common::Time stepTime); //NOT TESTED
    void sendImpPositionToGazebo ( const int j, const double des, gazebo::common::Time stepTime);
    void sendImpPositionsToGazebo ( yarp::sig::Vector& dess, gazebo::common::Time stepTime);
    void computeTrajectory(const int j);
    void prepareResetJointMsg(int j);
    
    bool isJointMotor(int j);

};

#endif //GAZEBOYARP_CONTROLBOARDDRIVER_HH
