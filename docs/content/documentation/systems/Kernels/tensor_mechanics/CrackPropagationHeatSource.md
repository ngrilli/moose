# CrackPropagationHeatSource
Provides a heat source from crack propagation:
$$
- \frac{d\Psi}{dc} \cdot \frac{dc}{dt}
$$
$\Psi$ is the free energy of the phase-field fracture model
defined as
$$
\Psi = (1 - c)^2 \cdot G_{0,+} + G_{0,-}
$$
$c$ is the order parameter for damage, continuous between 0 and 1
0 represents no damage, 1 represents fully cracked
$G_{0,+}$ and $G_{0,-}$ are the positive and negative components
of the specific strain energies
$$
- \frac{d\Psi}{dc} \cdot \frac{dc}{dt} = 2 \cdot (1 - c) \cdot G_{0,+} \cdot \frac{dc}{dt}
$$
\cite{miehe2015phasefield}
\cite{chakraborty2016intergranularfracture}
This is a Kernel that applies this heat source as a term in the heat equation.
This is intended to be used in conjunction with the [ComputeCrackPropagationHeatEnergy](/Materials/tensor_mechanics/ComputeCrackPropagationHeatEnergy.md) Material,
which computes the value of that source.

!syntax description /Kernels/CrackPropagationHeatSource

!syntax parameters /Kernels/CrackPropagationHeatSource

!syntax inputs /Kernels/CrackPropagationHeatSource

!syntax children /Kernels/CrackPropagationHeatSource