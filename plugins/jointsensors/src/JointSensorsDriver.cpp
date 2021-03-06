/*
 * Copyright (C) 2007-2013 Istituto Italiano di Tecnologia ADVR & iCub Facility
 * Authors: Enrico Mingo, Alessio Rocchi, Mirko Ferrati, Silvio Traversaro and Alessandro Settimi
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "JointSensorsDriver.h"
#include <GazeboYarpPlugins/Handler.hh>
#include <GazeboYarpPlugins/common.h>

using namespace yarp::dev;


GazeboYarpJointSensorsDriver::GazeboYarpJointSensorsDriver()
{}


GazeboYarpJointSensorsDriver::~GazeboYarpJointSensorsDriver()
{}


/**
 *
 * Export a group of joint sensors (position, speed or torque).
 * 
 * \todo check returned data
 */
void GazeboYarpJointSensorsDriver::onUpdate(const gazebo::common::UpdateInfo & _info)
{
    assert(joint_ptrs.size() == jointsensors_nr_of_channels);
    /** \todo ensure that the timestamp is the right one */
    last_timestamp.update(_info.simTime.Double());
    
    data_mutex.wait();
    for ( unsigned int jnt_cnt=0; jnt_cnt < joint_ptrs.size(); jnt_cnt++ )
    {
        switch(jointsensors_type) {
            case Position :
                //As convention in yarp ports, the angles are expressed in degrees
                jointsensors_data[jnt_cnt] = joint_ptrs[jnt_cnt]->GetAngle ( 0 ).Degree();
                break;
            case Speed :
                //As convention in yarp ports, the angular speeds are expressed in degrees/sec
                jointsensors_data[jnt_cnt] = 
                    GazeboYarpPlugins::convertRadiansToDegrees(joint_ptrs[jnt_cnt]->GetVelocity ( 0 ));
                break;
            case Torque : 
                //As convention in yarp ports, the torque are expressed in Nm
                jointsensors_data[jnt_cnt] = joint_ptrs[jnt_cnt]->GetForce ( 0u );
                break;
            default:
                assert(false);
                break;
        }
    }
    data_mutex.post();
    
    return;
}
    
//DEVICE DRIVER
bool GazeboYarpJointSensorsDriver::open(yarp::os::Searchable& config)
{
    std::cout << "GazeboYarpJointSensorsDriver::open() called" << std::endl;
  
    yarp::os::Property pluginParameters;
    pluginParameters.fromString(config.toString().c_str());

    std::string robotName (pluginParameters.find("robotScopedName").asString().c_str());
    std::cout << "DeviceDriver is looking for robot " << robotName << "...\n";

    _robot = GazeboYarpPlugins::Handler::getHandler()->getRobot(robotName);
    if(NULL == _robot)
    {
        std::cout << "GazeboYarpJointSensorsDriver error: robot was not found\n";
        return false;
    }
    
    bool ok = setJointPointers(pluginParameters);
    assert(joint_ptrs.size() == jointsensors_nr_of_channels);
    if( !ok ) 
    { 
        return false;
    }
    
    ok = setJointSensorsType(pluginParameters);
    if( !ok ) 
    { 
        return false;
    }
    
    data_mutex.wait();
    jointsensors_data.resize(jointsensors_nr_of_channels,0.0);
    data_mutex.post();
    
    //Connect the driver to the gazebo simulation
    this->updateConnection = gazebo::event::Events::ConnectWorldUpdateBegin (
                                 boost::bind ( &GazeboYarpJointSensorsDriver::onUpdate, this, _1 ) );
  
    std::cout << "GazeboYarpJointSensorsDriver::open() returning true" << std::endl;
    return true;
}

bool GazeboYarpJointSensorsDriver::setJointPointers(yarp::os::Property & pluginParameters)  //WORKS
{
    std::cout << ".ini file found, using joint names in ini file" << std::endl;
    yarp::os::Bottle joint_names_bottle = pluginParameters.findGroup("jointNames");

    if (joint_names_bottle.isNull()) {
        std::cout << "GazeboYarpJointSensorsDriver::setJointPointers() error: cannot find jointNames parameter." << std::endl;
        return false;
    }
    
    jointsensors_nr_of_channels = joint_names_bottle.size() - 1;
    
    if (jointsensors_nr_of_channels == 0) {
        std::cout << "GazeboYarpJointSensorsDriver::setJointPointers() error: no joint selected." << std::endl;
        return false;
    }
    
    joint_ptrs.resize(jointsensors_nr_of_channels);
    
    const gazebo::physics::Joint_V & gazebo_models_joints = _robot->GetJoints();

    for(unsigned int i=0; i < joint_ptrs.size(); i++ ) {
        bool joint_found = false;
        std::string controlboard_joint_name(joint_names_bottle.get(i+1).asString().c_str());
        
        std::string  controlboard_joint_name_scoped_ending = "::" + controlboard_joint_name;
        
        for(unsigned int gazebo_joint = 0; gazebo_joint < gazebo_models_joints.size() && !joint_found; gazebo_joint++ ) {
            std::string gazebo_joint_name = gazebo_models_joints[gazebo_joint]->GetScopedName();
            if( GazeboYarpPlugins::hasEnding(gazebo_joint_name,controlboard_joint_name_scoped_ending) ) {
                joint_found = true;
                joint_ptrs[i] = boost::get_pointer(gazebo_models_joints[gazebo_joint]);
            }
        }
        
        if( !joint_found ) { 
            std::cout << "GazeboYarpJointSensorsDriver::setJointPointers(): Error, cannot find joint " << controlboard_joint_name << std::endl;
            joint_ptrs.resize(0);
            jointsensors_nr_of_channels = 0;
            return false;
        }
       
    }     
    return true;
}

bool GazeboYarpJointSensorsDriver::setJointSensorsType(yarp::os::Property & pluginParameters)  //WORKS
{
    std::cout << ".ini file found, using joint names in ini file" << std::endl;
    
    std::string parameter_name = "gazeboJointSensorsType";
    
    if(!pluginParameters.check(parameter_name.c_str())) {
        std::cout << "GazeboYarpJointSensorsDriver::setJointSensorsType() error: cannot find " << parameter_name << " parameter." << std::endl;
        return false;
    }
    
    std::string sensors_type = pluginParameters.find(parameter_name.c_str()).asString().c_str();
    
    if( sensors_type == "position" ) {
        jointsensors_type = Position;
    } else if ( sensors_type == "speed" ) {
        jointsensors_type = Speed;
    } else if ( sensors_type == "torque" ) {
        jointsensors_type = Torque;
    } else {
        std::cerr << "GazeboYarpJointSensorsDriver::setJointSensorsType() error: sensor type " << sensors_type << " not recognized." << std::endl
                  << "\t\tThe available types are position, speed and torque." << std::endl;
        return false;
    }
    
    return true;
}



bool GazeboYarpJointSensorsDriver::close()
{
    gazebo::event::Events::DisconnectWorldUpdateBegin(this->updateConnection);
    return true;
}
    
//ANALOG SENSOR
int GazeboYarpJointSensorsDriver::read(yarp::sig::Vector &out)
{
    ///< \todo TODO in my opinion the reader should care of passing a vector of the proper dimension to the driver, but apparently this is not the case
    /*
    if( (int)jointsensors_data.size() != jointsensors_nr_of_channels ||
        (int)out.size() != jointsensors_nr_of_channels ) {
        return AS_ERROR;
    }
    */
    
    if( (int)jointsensors_data.size() != jointsensors_nr_of_channels ) {
        return AS_ERROR;
    }
   
    if( (int)out.size() != jointsensors_nr_of_channels ) {
        std::cout << " GazeboYarpJointSensorsDriver:read() warning : resizing input vector, this can probably be avoided" << std::endl;
        out.resize(jointsensors_nr_of_channels);
    }
      
    data_mutex.wait();
    out = jointsensors_data;
    data_mutex.post();
    
    return AS_OK;
}

int GazeboYarpJointSensorsDriver::getChannels()
{
    return jointsensors_nr_of_channels;
}

int GazeboYarpJointSensorsDriver::getState(int /*ch*/)
{
    printf("getstate\n");
    return AS_OK;
}

int GazeboYarpJointSensorsDriver::calibrateSensor()
{
    return AS_OK;
}

int GazeboYarpJointSensorsDriver::calibrateSensor(const yarp::sig::Vector& /*value*/)
{
    return AS_OK;
}

int GazeboYarpJointSensorsDriver::calibrateChannel(int /*ch*/)
{
    return AS_OK;
}

int GazeboYarpJointSensorsDriver::calibrateChannel(int /*ch*/, double /*v*/)
{
    return AS_OK;
}

//PRECISELY TIMED
yarp::os::Stamp GazeboYarpJointSensorsDriver::getLastInputStamp()
{
    return last_timestamp;
}
