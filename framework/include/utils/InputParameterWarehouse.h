/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef INPUTPARAMETERWAREHOUSE_H
#define INPUTPARAMETERWAREHOUSE_H

// MOOSE includes
#include "ParallelUniqueId.h"
#include "MooseObjectName.h"
#include "MooseTypes.h"
#include "ControllableParameter.h"

// Forward declarations
class InputParameters;

/**
 * Storage container for all InputParamter objects.
 *
 * This object is responsible for InputParameter objects, all MooseObjects should
 * contain a reference to the parameters object stored here.
 *
 */
class InputParameterWarehouse
{
public:

  /**
   * Class constructor
   */
  InputParameterWarehouse();

  /**
   * Destruction
   */
  virtual ~InputParameterWarehouse();

  ///@{
  /**
   * Return a const reference to the InputParameters for the named object
   * @param tag The tag of the object (e.g., 'Kernel')
   * @param name The name of the parameters object, including the tag (name only input) or MooseObjectName object
   * @param tid The thread id
   * @return A const reference to the warehouse copy of the InputParameters
   */
  const InputParameters & getInputParametersObject(const std::string & name, THREAD_ID tid = 0) const;
  const InputParameters & getInputParametersObject(const std::string & tag, const std::string & name, THREAD_ID tid = 0 ) const;
  const InputParameters & getInputParametersObject(const MooseObjectName & object_name, THREAD_ID tid = 0 ) const;
  ///@{


  /**
   * Returns a ControllableParameter object
   * @see Control
   */
  template<typename T>
  ControllableParameter<T> getControllableParameter(const MooseObjectParameterName & desired) const;

  void addControllableParameterConnection(const MooseObjectParameterName & master, const MooseObjectParameterName & slave);

private:

  /// Storage for the InputParameters objects
  std::vector<std::multimap<MooseObjectName, MooseSharedPointer<InputParameters> > > _input_parameters;

  /// InputParameter links
  std::map<MooseObjectParameterName, std::vector<MooseObjectParameterName> > _input_parameter_links;

  /**
   * Method for adding a new InputParameters object
   * @param parameters The InputParameters object to copy and store in the warehouse
   * @return A reference to the warehouse copy of the InputParameters, this
   *         is what should be passed into the MooseObjects constructors.
   *
   * A new object is created from the old object because InputParameters objects
   * are generic until Factory::create() is called and the actual MooseObject
   * is created.
   *
   * This method is private, because only the factories that are creating objects should be
   * able to call this method.
   */
  InputParameters & addInputParameters(const std::string & name, InputParameters parameters, THREAD_ID tid = 0);

  ///@{
  /**
   * Return a reference to the InputParameters for the named object
   * @param tag The tag of the object (e.g., 'Kernel')
   * @param name The name of the parameters object, including the tag (name only input) or MooseObjectName object
   * @param tid The thread id
   * @return A reference to the warehouse copy of the InputParameters
   *
   * If you are using this method to access a writable reference to input parameters, this
   * will break the ability to control the parameters with the MOOSE control logic system.
   * Only change parameters if you know what you are doing. Hence, this is private for a reason.
   */
  InputParameters & getInputParameters(const std::string & name, THREAD_ID tid = 0) const;
  InputParameters & getInputParameters(const std::string & tag, const std::string & name, THREAD_ID tid = 0 ) const;
  InputParameters & getInputParameters(const MooseObjectName & object_name, THREAD_ID tid = 0 ) const;
  ///@{


  friend class Factory;
  friend class ActionFactory;
  friend class Control;

  // RELAP-7 Control Logic (This will go away when the MOOSE system is created)
  friend class Component;
  friend class R7SetupOutputAction;
  friend class SolidMaterialProperties;
};


template<typename T>
ControllableParameter<T>
InputParameterWarehouse::getControllableParameter(const MooseObjectParameterName & input) const
{
  // The ControllableParameter object to return
  ControllableParameter<T> output;

  // Vector of desired parameters
  std::vector<MooseObjectParameterName> params(1, input);
  std::map<MooseObjectParameterName, std::vector<MooseObjectParameterName> >::const_iterator link_it = _input_parameter_links.find(input);
  if (link_it != _input_parameter_links.end())
    params.insert(params.end(), link_it->second.begin(), link_it->second.end());

  // Parameter object iterator
  std::multimap<MooseObjectName, MooseSharedPointer<InputParameters> >::const_iterator iter;

  // Loop over all threads
  for (THREAD_ID tid = 0; tid < libMesh::n_threads(); ++tid)
  {
    // Loop over all InputParameter objects
    for (iter = _input_parameters[tid].begin(); iter != _input_parameters[tid].end(); ++iter)
    {
      // Loop of all desired params
      for (std::vector<MooseObjectParameterName>::const_iterator it = params.begin(); it != params.end(); ++it)
      {
        // If the desired object name does not match the current object name, move on
        const MooseObjectParameterName & desired = *it;

        if (desired != iter->first)
          continue;

        // If the parameter is valid and controllable update the output vector with a pointer to the parameter
        if (iter->second->libMesh::Parameters::have_parameter<T>(desired.parameter()))
        {
          // Do not allow non-controllable types to be controlled
          if (!iter->second->isControllable(desired.parameter()))
            mooseError("The desired parameter is not controllable: " << desired);

          // Store pointer to the writable parameter
          output.insert(MooseObjectParameterName(iter->first, desired.parameter()), &(iter->second->set<T>(desired.parameter())));
        }
      }
    }
  }

  // Error if nothing was found
  if (output.size() == 0)
    mooseError("The controlled parameter was not found: " << input);

  return output;
}

#endif // INPUTPARAMETERWAREHOUSE_H
