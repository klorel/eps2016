/********************************************************
  Xpress-BCL Java Example Problems
  ================================

  file xbcutex.java
  `````````````````
  Simplified version of xbexpl1.java showing how 
  to define cuts with BCL.

  (c) 2008 Fair Isaac Corporation
      author: S.Heipcke, 2005, rev. Dec. 2011
********************************************************/

import java.io.*;
import com.dashoptimization.*;

public class xbcutex
{
 static final int NJ = 4;        /* Number of jobs */   
 static final int NT = 10;       /* Time limit */   

/**** DATA ****/
 static final double[] DUR = {3,4,2,2};   /* Durations of jobs   */ 

 static XPRBvar[] start;         /* Start times of jobs  */ 
 static XPRBvar[][] delta;       /* Binaries for start times */  
 static XPRBvar z;               /* Maximum completion time (makespan) */ 

 static XPRB bcl;
 static XPRBprob p; 
             
/*************************************************************************/

 static class CutMgrCallback implements XPRScutMgrListener
 {
  public int XPRScutMgrEvent(XPRSprob oprob, Object data)
  {
   XPRBprob bprob;
   XPRBcut[] ca;
   int num,i;

   bprob = (XPRBprob)data;       /* Get the BCL problem */

   try 
   {   
    bprob.beginCB(oprob);
    num = oprob.getIntAttrib(XPRS.NODES);
    if(num == 2)                 /* Only generate cuts at node 2 */
    {
     ca = new XPRBcut[2];
     ca[0] = bprob.newCut(start[1].add(2) .lEql(start[0]), 2); 
     ca[1] = bprob.newCut(start[2].mul(4) .add(start[3].mul(-5.3)) .lEql(-17), 2);
     System.out.println("Adding constraints:");
     for(i=0;i<2;i++) ca[i].print();
     bprob.addCuts(ca);
    }
    bprob.endCB();
   }
   catch(XPRSprobException e)
   {
    System.out.println("Error  " + e.getCode() + ": " + e.getMessage());
   } 
   return 0;                     /* Call this method once per node */
  }
 }

/*************************************************************************/

 static void jobsModel()
 {
  XPRBexpr le;
  int j,t;

/****VARIABLES****/
  start = new XPRBvar[NJ];       /* Create start time variables */
  for(j=0;j<NJ;j++) start[j] = p.newVar("start");
  z = p.newVar("z",XPRB.PL,0,NT);  /* Declare the makespan variable */

  delta = new XPRBvar[NJ][NT];
  for(j=0;j<NJ;j++)              /* Declare binaries for each job  */
   for(t=0;t<(NT-DUR[j]+1);t++)
    delta[j][t] = p.newVar("delta"+(j+1)+(t+1), XPRB.BV);

/****CONSTRAINTS****/
  for(j=0;j<NJ;j++)              /* Calculate maximal completion time */
   p.newCtr("Makespan", start[j].add(DUR[j]).lEql(z) );
  
  p.newCtr("Prec", start[0].add(DUR[0]).lEql(start[2]) );
                                 /* Precedence rel. between jobs */

  for(j=0;j<NJ;j++)              /* Linking start times and binaries  */ 
  { 
   le = new XPRBexpr();
   for(t=0;t<(NT-DUR[j]+1);t++)  le.add(delta[j][t].mul((t+1))); 
   p.newCtr("Link_"+(j+1), le.eql(start[j]) );
  } 
               
  for(j=0;j<NJ;j++)              /* One unique start time for each job  */
  { 
   le = new XPRBexpr();
   for(t=0;t<(NT-DUR[j]+1);t++)  le.add(delta[j][t]); 
   p.newCtr("One_"+(j+1), le.eql(1));
  }      
              
/****OBJECTIVE****/
  p.setObj(z);                   /* Define and set objective function */ 

/****BOUNDS****/
  for(j=0;j<NJ;j++) start[j].setUB(NT-DUR[j]+1); 
                                 /* Upper bounds on start time variables */

/****OUTPUT****/
  p.print();                     /* Print out the problem definition */ 
 } 

/*************************************************************************/

 static void jobsSolve() throws XPRSexception       
 { 
  int j,t,statmip; 
  XPRSprob oprob;
  CutMgrCallback cb;

  oprob = p.getXPRSprob();        /* Get Optimizer problem */
  oprob.setIntControl(XPRS.HEURSTRATEGY, 0);
  oprob.setIntControl(XPRS.CUTSTRATEGY, 0);
                                  /* Switch automated cut generation off:
                                     otherwise this problem is solved in the
                                     first node of the MIP search tree */

  p.setCutMode(1);                /* Switch presolve off, enable cut manager */
  cb = new CutMgrCallback();
  oprob.addCutMgrListener(cb, p);

  p.setSense(XPRB.MINIM);
  p.mipOptimize("");              /* Solve the problem as MIP */
  statmip = p.getMIPStat();       /* Get the MIP problem status */    
              
  if((statmip == XPRB.MIP_SOLUTION) || (statmip == XPRB.MIP_OPTIMAL))
                                  /* An integer solution has been found */
  {  
   System.out.println("Objective: "+ p.getObjVal()); 
   for(j=0;j<NJ;j++) 
   {                              /* Print the solution for all start times */
    System.out.println(start[j].getName() + ": "+ start[j].getSol()); 
    for(t=0;t<NT-DUR[j]+1;t++) 
     System.out.print(delta[j][t].getName() + ": "+ delta[j][t].getSol());
    System.out.println();
   }
  } 
 }

/*************************************************************************/

 public static void main(String[] args) throws XPRSexception
 {         
  bcl = new XPRB();              /* Initialize BCL */
  p = bcl.newProb("Jobs");       /* Create a new problem */
  XPRS.init();                   /* Initialize Xpress-Optimizer */
  
  jobsModel();                   /* Basic problem definition */
    
  try
  {
   jobsSolve();                  /* Solve and print solution */
  }
  catch(XPRSexception e)
  {
   System.err.println(e.getMessage());
   System.exit(1);
  }

  p.finalize();                  /* Delete the problem */           
  p=null;
 }
}
