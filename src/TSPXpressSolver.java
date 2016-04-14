import com.dashoptimization.*;
import javafx.util.Pair;

import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.Map;

/**
 * Created by mruiz on 13/04/2016.
 */
public class TSPXpressSolver implements XPRSmessageListener, XPRSpreIntsolListener, XPRScutMgrListener {
    public XPRB bcl;
    public XPRBprob problem;
    public TSPInstance instance;

    public LinkedHashMap<Pair<Integer, Integer>, XPRBvar> variables = new LinkedHashMap<>();
    public LinkedList<XPRBcut> cuts = new LinkedList<>();

    public TSPXpressSolver(TSPInstance instance) {
        this.instance = instance;
    }

    XPRBvar getVar(Integer i, Integer j) {
        return variables.get(new Pair<>(i < j ? i : j, i < j ? j : i));
    }

    XPRBprob buildProblem() {
        bcl = new XPRB();
        // Create a new problem in BCL
        problem = bcl.newProb(instance.toString());
//
//        XPRB.setMsgLevel(4);
//        problem.setMsgLevel(4);
        problem.setCutMode(1);
        problem.getXPRSprob().addMessageListener(this);
        problem.getXPRSprob().addPreIntsolListener(this);
        problem.getXPRSprob().addCutMgrListener(this);

        variables = new LinkedHashMap<>();
        XPRBexpr obj = new XPRBexpr();
        // variable x[i,j], i<j
        for (Integer i = 0; i < instance.nPoints; ++i) {
            for (Integer j = i + 1; j < instance.nPoints; ++j) {
                XPRBvar var = problem.newVar("x_" + i.toString() + "_" + j.toString(), XPRB.BV);
                variables.put(new Pair(i, j), var);
                obj.add(var.mul(instance.distance[i][j]));
            }
        }
        problem.setObj(obj);
        // degree constraint
        for (Integer i = 0; i < instance.nPoints; ++i) {
            XPRBexpr degree = new XPRBexpr();
            for (Integer j = 0; j < instance.nPoints; ++j) {
                if (i != j) {
                    degree.add(getVar(i, j));
                }
            }
            problem.newCtr("degree_" + i.toString(), degree.eql(2.0));
        }
        return problem;
    }

    public void XPRSmessageEvent(XPRSprob prob,
                                 Object data, String msg, int len, int type) {
        System.out.println(msg);
    }

    public TSPInstance getInstance() {
        return instance;
    }

    LinkedList<LinkedList<Integer>> getSubtour() {
        LinkedList<LinkedList<Integer>> result = new LinkedList<>();
        int[] in_sub_tour = new int[getInstance().nPoints];
        LinkedList<Integer>[] sub_tour = new LinkedList[getInstance().nPoints];
        for (int i = 0; i < getInstance().nPoints; ++i) {
            in_sub_tour[i] = i;
            sub_tour[i] = new LinkedList<>();
            sub_tour[i].add(i);
        }
        for (Map.Entry<Pair<Integer, Integer>, XPRBvar> kvp : variables.entrySet()) {
            final int i = kvp.getKey().getKey();
            final int j = kvp.getKey().getValue();
            final int sub_i = in_sub_tour[i];
            final int sub_j = in_sub_tour[j];
            final double solValue = kvp.getValue().getSol();
            if (solValue > 1 - 1e-10 && in_sub_tour[i] != in_sub_tour[j]) {
//                System.out.println("--- begin ---");
//                System.out.println(sub_tour[sub_i].toString());
//                System.out.println(sub_tour[sub_j].toString());
                LinkedList<Integer> clone_tour = new LinkedList<>(sub_tour[sub_j]);
                sub_tour[sub_j].clear();
                sub_tour[sub_i].addAll(clone_tour);
                for (int v : clone_tour) {
                    in_sub_tour[v] = sub_i;
                }
//                System.out.println(sub_tour[sub_i].toString());
//                System.out.println("---   end ---");
            }
        }
        for (LinkedList<Integer> tour : sub_tour) {
            if (tour.size() > 0) {
                result.add(tour);
            }
        }
        System.out.println("Number of sub tour " + result.size());
        return result;
    }

    XPRBexpr getCut(LinkedList<Integer> tour) {
        XPRBexpr xprBexpr = new XPRBexpr();
        for (int i : tour) {
            for (int j : tour) {
                if (i < j) {
                    xprBexpr.addTerm(getVar(i, j), 1);
                }
            }
        }
        return xprBexpr;
    }

    void breakSubTour(LinkedList<LinkedList<Integer>> sub_tour) {
        try {
            LinkedList<XPRBcut> cuts = new LinkedList<>();
            for (LinkedList<Integer> tour : sub_tour) {
                if (tour.size() > 1) {
                    problem.newCtr(getCut(tour).lEql(tour.size() - 1.0));
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void XPRSpreIntsolEvent(XPRSprob xprSprob, Object o, boolean b, IntHolder intHolder, DoubleHolder doubleHolder) {
//        System.out.println("XPRSpreIntsolEvent");
//
//        try {
//            problem.beginCB(xprSprob);
//            XPRBprob p = (XPRBprob) o;
//            problem.sync(XPRB.XPRS_SOL);
//            LinkedList<LinkedList<Integer>> sub_tour = getSubtour();
//            cuts.clear();
//            for (LinkedList<Integer> tour : sub_tour) {
//                if (tour.size() > 1) {
//                    this.cuts.add(problem.newCut(getCut(tour).lEql(tour.size() - 1.0)));
//                }
//            }
////            if (cuts.size() > 1) {
//////                XPRSaddcuts(XPRSprob prob, int ncuts, const int mtype[], const
//////                char qrtype[], const double drhs[], const int mstart[], const int
//////                mcols[], const double dmatval[]);
////                int ncuts = 1;
////                int[] mtype = new int[ncuts];
////                byte[] qrtype = new byte[ncuts];
////                double[] drhs = new double[ncuts];
////                int[] mstart = new int[ncuts + 1];
////
////                mtype[0] = 1;
////                qrtype[0] = 'L';
////                drhs[0] = 0;
////                mstart[0] = 0;
////                mstart[1] = 0;
////
////                int[] mcols = new int[ncuts];
////                double[] dmatval = new double[ncuts];
////                mcols[0] = 0;
////                dmatval[0] = 1;
////                xprSprob.addCuts(ncuts, mtype, qrtype, drhs, mstart, mcols, dmatval);
////                problem.addCuts(cuts.toArray(new XPRBcut[cuts.size()]));
////            }
//            problem.endCB();
//        } catch (Exception e) {
//            e.printStackTrace();
//        }
    }

    @Override
    public int XPRScutMgrEvent(XPRSprob xprSprob, Object o) {
        System.out.println("XPRScutMgrEvent");
        if (cuts.size() > 0) {
            problem.beginCB(xprSprob);
            XPRBprob p = (XPRBprob) o;
            problem.addCuts(cuts.toArray(new XPRBcut[cuts.size()]));
            problem.endCB();
        }
        return 0;
    }
}
