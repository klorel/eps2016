/**********************************************************************************\
* Name:        Trimloss.c                                   Fair Isaac 14/03/2014  *
* Purpose:     Modify a problem by adding an extra variable within an additional   *
*              constraint.                                                         *
* Description: We take a trimloss problem in which each integer variable x(p)      *
*              represents the number of rolls cut to pattern p.                    *
*              We define a new integer variable y=SUM(p)x(p) and add the associated*
*              constraint                                                          *
*                                   x(1)+x(2)+...+x(N)-y = 0                       *
*              We do this by first adding a row containing the (unitary)           *
*              coefficients of the x(p), and then a column corresponding to y.     *
*              We output the revised matrix to a file and then solve the revised   *
*              MIP, giving y the highest branching priority. Finally, we output    *
*              the solution, both to the screen and to an ASCII solution file.     *
* Input:       trimloss.mat      original matrix file                              *
* Output:      trimloss.log      log file                                          *
*              Trimloss.sol      binary solution file - produced by default        *
*              Revised.mps       revised matrix file                               *
*              Revised.asc       ASCII solution file                               *
*              Revised.hdr       header for the ASCII solution file                *
\**********************************************************************************/

import java.io.*;
import java.text.DecimalFormat;

import com.dashoptimization.*;



public class Trimloss {
	/**
	 * Execute example from command-line
	 **/
	public static void main(String[] args) {
		Trimloss trimloss = null;
		XPRS.init();
		try {
			System.out.println(XPRS.getBanner());
			trimloss = new Trimloss();
			trimloss.run();
		} finally {
			if (trimloss!=null) trimloss.destroy();
			XPRS.free();
		}
	}
	
	
	/**
	 * XPRSprob for the trimloss problem
	 **/
	private XPRSprob probg;
	
	/**
	 * The index of the newly-added now
	 **/
	private int addedRowIndex;
	
	/**
	 * The index of the newly-added column
	 **/
	private int addedColIndex;
	

	/**
	 * Initialize Trimloss problem
	 **/
	public Trimloss() {
		probg = new XPRSprob();
	}
	
	/**
	 * Destroy the Trimloss problem
	 **/
	public void destroy() {
		probg.destroy();
	}
	
	/**
	 * Execute Trimloss example
	 **/
	public void run() {
		/* Setup the logfile */
		File logfile = new File("trimloss.log");
		if (logfile.exists()) logfile.delete();
		probg.setLogFile(logfile.toString());
		
		/* Print Xpress version number */
		DecimalFormat versionNumberFormatter = new DecimalFormat("0.00");
		System.out.println("Xpress Optimizer Subroutine Library Release "+versionNumberFormatter.format( (double)probg.controls().getVersion()/100 ));

		/* Load initial problem */
		loadInitialProblem();
		
		/* Add new row */
		addNewRow();
		
		/* Add new column */
		addNewColumn();
		
		/* Output the revised matrix */
		probg.writeProb("revised");
		
		/* Solve the MIP */
		solveProblem();
		
		/* Display solution */
		printSolution();
		
		/* Write solution to an ASCII file */
		probg.writeSol("revised");
		
	}


	/**
	 * Loads the initial trimloss matrix into the Xpress-Optimizer problem
	 **/
	private void loadInitialProblem() {
		probg.readProb("../data/trimloss");
	}
	
	/**
	 * Adds a new row to the matrix
	 **/
	private void addNewRow() {
		int nCol = probg.attributes().getCols(); /* nCol = number of columns in the matrix */
		int nNewRows = 1; /* Adding 1 new row */
		double[] dRHS = {0.0};
		int[] nRowStart = {0};
		byte[] sRowType = {'E'};
		/* Store the coefficients of x(p); */
		int nRowElem = nCol; /* New row has a coefficient in every column */
		double[] pRowVal = new double[nRowElem];
		int[] pColInd = new int[nRowElem];
		for (int i=0;i<nCol;i++) {
			pRowVal[i] = 1.0;
			pColInd[i] = i;
		}
		/* Add the new row */
		probg.addRows( nNewRows, nRowElem, sRowType, dRHS, null, nRowStart, pColInd, pRowVal );
	
		/* New row will have been added at end of matrix */
		this.addedRowIndex = probg.attributes().getRows() - 1;
		
		/* Set new row name */
		String[] newNames = {"NewRow"};
		probg.addNames( 1 /* 1 = row names */ , newNames, addedRowIndex, addedRowIndex );
	}
	
	/** 
	 * Adds a new column to the matrix
	 **/
	private void addNewColumn() {
		int nRow = probg.attributes().getRows(); /* nRow = number of rows in the matrix */
		int nNewCols = 1; /* Adding 1 new column */
		int nColElem = 1; /* New column has a single nonzero */
		double[] dObjCoef = {0.0};
		int[] nColStart = {0};
		double[] pColVal = {-1.0}; /* The coefficient of -y */
		int[] pRowInd = {nRow-1};
		double[] dLowerBnd = {0.0};
		double[] dUpperBnd = {90.0}; /* The sum of the bounds on the x(p) */
		
		/* Add the new column */
		probg.addCols( nNewCols, nColElem, dObjCoef, nColStart, pRowInd, pColVal, dLowerBnd, dUpperBnd );
		
		/* New column will have been added at the end of the matrix */
		this.addedColIndex = probg.attributes().getCols() - 1;
		
		/* Make the new column an integer variable */
		int nTypeChg = 1;
		int[] nNewColInd = {addedColIndex};
		byte[] sColType = {'I'};
		probg.chgColType( nTypeChg, nNewColInd, sColType );
		
		/* Set the new column name */
		String[] newNames = {"y"};
		probg.addNames( 2 /* 2 = column names */, newNames, addedColIndex, addedColIndex );
	}
	
	/**
	 * Solves the mixed-integer problem
	 **/
	private void solveProblem() {
		/* Turn off presolve and permit no cuts */
		probg.controls().setPresolve(0);
		probg.controls().setCutStrategy(0);

		/* Assign the y variable a high branching priority (1)
		   - by default the original x(p) variables have low priority (500) */
		int[] nNewColIndex = {addedRowIndex};
		int[] nColPrior = {1};
		/* Load this directive */
		probg.loadDirs( 1, nNewColIndex, nColPrior, null, null, null );
		
		probg.mipOptimize();
		
	}
	
	/** 
	 * Displays the solution
	 **/
	private void printSolution() {
		if (probg.attributes().getMIPStatus()==XPRSenumerations.MIPStatus.OPTIMAL) {
			System.out.println("  Problem solved to optimality");

			int nCol = probg.attributes().getCols();
			/* Allocate memory for the solution array */
			double[] x = new double[ nCol ];
			/* Get the primal solution values */
			probg.getSol(x);

			/* Display the objective and solution values */
			System.out.println();
			System.out.println("  Minimum loss is " + probg.attributes().getMIPObjVal());
			System.out.println();
			System.out.println("  The solution values are");
			for (int i=0;i<nCol-1;i++)
				System.out.println("    x("+(i+1)+") = "+x[i]);
			System.out.println("    y    = "+x[nCol-1]);
		}
		else {
			System.out.println("  Problem was "+probg.attributes().getMIPStatus());
		}
	}
		
}