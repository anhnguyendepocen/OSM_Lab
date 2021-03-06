#======================================================================
#
#     This routine interfaces with the TASMANIAN Sparse grid
#     The crucial part is
#
#     aVals[iI]=solveriter.iterate(aPoints[iI], n_agents)[0]
#     => at every gridpoint, we solve an optimization problem
#
#     Simon Scheidegger, 11/16 ; 07/17
#======================================================================

import TasmanianSG
import numpy as np
from parameters import *
import nonlinear_solver_iterate as solveriter

#======================================================================

def sparse_grid_iter(n_agents, iDepth, valold, theta):
    grid1 = TasmanianSG.TasmanianSparseGrid()

    k_range=np.array([k_bar, k_up])

    ranges=np.empty((n_agents, 2))

    for i in range(n_agents):
        ranges[i]=k_range

    iDim=n_agents
    iOut=1

    # Level of grid before refinement
    grid1.makeLocalPolynomialGrid(iDim, iOut, iDepth, which_basis, "localp")
    grid1.setDomainTransform(ranges)

    aPoints=grid1.getPoints()
    iNumP1=aPoints.shape[0]
    aVals=np.empty([iNumP1, 1])

    for iI in range(iNumP1):
        aVals[iI]=solveriter.iterate(aPoints[iI], n_agents, valold, theta)[0]
        v=aVals[iI]*np.ones((1,1))

    grid1.loadNeededPoints(aVals)

    for iK in range(refinement_level):
        grid1.setSurplusRefinement(fTol, 1, "fds")   #also use fds, or other rules
        aPoints = grid1.getNeededPoints()
        aVals = np.empty([aPoints.shape[0], 1])
        for iI in range(iNumP1):
            aVals[iI]=solveriter.iterate(aPoints[iI], n_agents, valold, theta)[0]
            v=aVals[iI]*np.ones((1,1))
        grid1.loadNeededPoints(aVals)
    return grid1

#======================================================================
