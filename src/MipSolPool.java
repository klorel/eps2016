/***************************************************************************************\
* Name:        MipSolPool.java                                   Fair Isaac 13/06/2008 *
* Purpose:     All solutions with the MIP solution pool                                *
* Description: We take the power generation problem stored in hpw15.mat which seeks to *
*              optimise the operating pattern of a group of electricity generators. We *
*              solve the problem collecting all solutions found during the MIP search. *
*              The optimal solution's objective and solution values are printed to     *
*              screen.                                                                 *
* Input:       hpw15.mat                                                               *
\***************************************************************************************/

import java.io.*;
import com.dashoptimization.*;

public class MipSolPool {
	public static void main(String[] args) throws Exception {
		XPRS.init();

		try {
			XPRSprob prob = new XPRSprob();
			prob.addMsgHandlerListener(System.out);

			XPRSmipsolpool msp = new XPRSmipsolpool();

			msp.probAttach(prob);

			prob.readProb("../data/hpw15");

			prob.minim("g");

			int nSols = msp.attributes().getSolutions();

			if (nSols>0) {
				IntHolder iSolutionId = new IntHolder();
				DoubleHolder dObj = new DoubleHolder();
				msp.getDblAttribProbExtreme(prob,0,iSolutionId, XPRS.MSP_SOLPRB_OBJ, dObj);

				System.out.println("Optimal Solution ID: " + iSolutionId.value);
				System.out.println("Optimal Objective  : " + dObj.value);

				int nCols = msp.getIntAttribSol(iSolutionId.value, null, XPRS.MSP_SOL_COLS);
				for (int i=0;i<nCols;i++) {
					double[] dSol = new double[1];
					msp.getSol(iSolutionId.value,null,dSol,i,i,null);
					System.out.println(i + " = " + dSol[0]);
				}
			}

		} finally {
			XPRS.free();
		}
	}
}