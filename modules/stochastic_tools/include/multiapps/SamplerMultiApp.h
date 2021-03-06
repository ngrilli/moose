/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef SAMPLERMULTIAPP_H
#define SAMPLERMULTIAPP_H

// MOOSE includes
#include "TransientMultiApp.h"
#include "SamplerInterface.h"

class SamplerMultiApp;

template <>
InputParameters validParams<SamplerMultiApp>();

class SamplerMultiApp : public TransientMultiApp, public SamplerInterface
{
public:
  SamplerMultiApp(const InputParameters & parameters);

protected:
  /// Sampler to utilize for creating MultiApps
  Sampler & _sampler;
};

#endif
