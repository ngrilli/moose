/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "PorousFlow1PhaseFullySaturated.h"

template <>
InputParameters
validParams<PorousFlow1PhaseFullySaturated>()
{
  InputParameters params = validParams<PorousFlowVariableBase>();
  params.addRequiredCoupledVar("porepressure",
                               "Variable that represents the porepressure of the single phase");
  params.addClassDescription("This Material is used for the fully saturated single-phase situation "
                             "where porepressure is the primary variable");
  return params;
}

PorousFlow1PhaseFullySaturated::PorousFlow1PhaseFullySaturated(const InputParameters & parameters)
  : PorousFlowVariableBase(parameters),

    _porepressure_var(_nodal_material ? coupledNodalValue("porepressure")
                                      : coupledValue("porepressure")),
    _gradp_qp_var(coupledGradient("porepressure")),
    _porepressure_varnum(coupled("porepressure")),
    _p_var_num(_dictator.isPorousFlowVariable(_porepressure_varnum)
                   ? _dictator.porousFlowVariableNum(_porepressure_varnum)
                   : 0)
{
  if (_num_phases != 1)
    mooseError("The Dictator proclaims that the number of phases is ",
               _dictator.numPhases(),
               " whereas PorousFlow1PhaseFullySaturated can only be used for 1-phase simulations."
               " Be aware that the Dictator has noted your mistake.");
}

void
PorousFlow1PhaseFullySaturated::initQpStatefulProperties()
{
  PorousFlowVariableBase::initQpStatefulProperties();
  buildQpPPSS();
}

void
PorousFlow1PhaseFullySaturated::computeQpProperties()
{
  // Size vectors correctly and prepare the derivative matrices with zeroes
  PorousFlowVariableBase::computeQpProperties();

  buildQpPPSS();

  if (!_nodal_material)
    (*_gradp_qp)[_qp][0] = _gradp_qp_var[_qp];

  // _porepressure is only dependent on _porepressure, and its derivative is 1
  if (_dictator.isPorousFlowVariable(_porepressure_varnum))
  {
    // _porepressure is a PorousFlow variable
    _dporepressure_dvar[_qp][0][_p_var_num] = 1.0;
    if (!_nodal_material)
      (*_dgradp_qp_dgradv)[_qp][0][_p_var_num] = 1.0;
  }
}

void
PorousFlow1PhaseFullySaturated::buildQpPPSS()
{
  _porepressure[_qp][0] = _porepressure_var[_qp];
  _saturation[_qp][0] = 1.0;
}
