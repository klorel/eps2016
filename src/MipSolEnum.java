/***************************************************************************************\
* Name:        MipSolEnum.java                                   Fair Isaac 13/06/2008 *
* Purpose:     10 best solutions with the MIP solution enumerator                      *
* Description: We take the power generation problem stored in hpw15.mat which seeks to *
*              optimise the operating pattern of a group of electricity generators. We *
*              run the MIP solution enumerator on the problem using the default setup  *
*              obtaining the best 10 solutions. The best 10 solutions are stored to a  *
*              MIP solution pool.  The solutions' objectives and solution values are   *
*              printed to screen.                                                      *
* Input:       hpw15.mat                                                               *
\***************************************************************************************/

import java.io.*;
import com.dashoptimization.*;

public class MipSolEnum {
	public static void main(String[] args) throws Exception {
		XPRS.init();
		
		try {

			XPRSprob prob = new XPRSprob();
			prob.addMsgHandlerListener(System.out);

			XPRSmipsolpool msp = new XPRSmipsolpool();
			XPRSmipsolenum mse = new XPRSmipsolenum();

			prob.readProb("hpw15");
			int nCols = prob.attributes().getCols();

			/* Disable heuristics to avoid duplicate solutions being stored */
			prob.controls().setHeurStrategy(0);

                        int iPresolveOps = prob.controls().getPresolveOps();
                        iPresolveOps &= ~(1 << 3); /* Disable dual reduction operations */
                        iPresolveOps &= ~(1 << 5); /* Disable duplicate column removal  */
                        prob.controls().setPresolveOps(iPresolveOps);

                        int iMIPPresolve = prob.controls().getMIPPresolve();
                        iMIPPresolve &= ~(1 << 4); /* Disable MIP dual reduction operations */
                        prob.controls().setMIPPresolve(iMIPPresolve);

			/* Run the enumeration */
			IntHolder nMaxSols = new IntHolder(10);
			mse.minim(prob,msp,new XPRSdefaultMipSolEnumHandler(), null, nMaxSols);

			/* Print out the solutions found */
			int nSols = mse.attributes().getSolutions();
			for (int i=1;i<=nSols;i++) {
				int[] iSolutionId = new int[1];
				DoubleHolder dObj = new DoubleHolder();
				mse.getSolList(XPRS.MSE_METRIC_MIPOBJECT,i,i,iSolutionId,null,null);
				mse.getSolMetric(iSolutionId[0], null, XPRS.MSE_METRIC_MIPOBJECT, dObj);
				System.out.println("--------\n" + i + " = " + dObj.value);
				for (int j=0;j<nCols;j++) {
					double[] dSol = new double[1];
					msp.getSol(iSolutionId[0],null,dSol,j,j,null);
					System.out.println(j + " = " + dSol[0]);
				}
			}

		} finally {
			XPRS.free();
		}
	}
}
