import com.dashoptimization.*;
import javafx.util.Pair;

import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.Map;

/**
 * Created by mruiz on 13/04/2016.
 */
public class TSPXpressSolver implements XPRSmessageListener, XPRSpreIntsolListener, XPRScutMgrListener, XPRSoptNodeListener {
    public XPRB bcl;
    public XPRBprob problem;
    public TSPInstance instance;

    public LinkedHashMap<Pair<Integer, Integer>, XPRBvar> variables = new LinkedHashMap<>();
    public LinkedList<XPRBcut> cuts = new LinkedList<>();
    public int cutId = 0;

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
        XPRS.init();

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
        System.out.println("XPRSmessageEvent  : " + msg);
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
        return result;
    }

    XPRBexpr getCut(LinkedList<Integer> tour) {
        XPRBexpr xprBexpr = new XPRBexpr();
        double cutValue = 0;
        for (int i : tour) {
            for (int j : tour) {
                if (i < j) {
                    xprBexpr.addTerm(getVar(i, j), 1);
                    cutValue += getVar(i, j).getSol();
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

    public void generateCut(XPRBprob bprob, XPRSprob oprob) {
        LinkedList<LinkedList<Integer>> sub_tour = getSubtour();
        cuts.clear();
        for (LinkedList<Integer> tour : sub_tour) {
            if (tour.size() > 1) {
                final double rhs = (tour.size() - 1.0);
                XPRBexpr expr = getCut(tour).add(rhs);
//                    expr.print();
                cutId += 1;
                this.cuts.add(bprob.newCut(expr.lEql(0), cutId));
            }
        }
//        System.out.println("sub_tour.size() = "+sub_tour.size());
//        for (XPRBcut cut : cuts)
//            cut.print();

    }

    @Override
    public void XPRSpreIntsolEvent(XPRSprob oprob, Object o, boolean b, IntHolder intHolder, DoubleHolder doubleHolder) {
        try {
            XPRBprob bprob = (XPRBprob) o;
            bprob.beginCB(oprob);
            bprob.sync(XPRB.XPRS_SOL);

            generateCut(bprob, oprob);

            if (cuts.size() > 1) {
                int num = oprob.getIntAttrib(XPRS.NODES);
                intHolder.value = 1;
                System.out.println(String.format("XPRSpreIntsolEvent  : %4d cuts generated at node %4d", cuts.size(), num));
            } else {
            }
            bprob.endCB();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public int XPRScutMgrEvent(XPRSprob oprob, Object o) {
        XPRBprob bprob = (XPRBprob) o;
        bprob.beginCB(oprob);
        int num = oprob.getIntAttrib(XPRS.NODES);
        System.out.println(String.format("XPRScutMgrEvent  : %4d  cuts at node %4d", oprob.getIntAttrib(XPRS.CUTS), num));
//        if (cuts.size() > 0) {
//            int num = oprob.getIntAttrib(XPRS.NODES);
//            System.out.println(String.format("XPRScutMgrEvent  : adding cut to node %4d", num));
//            bprob.addCuts(cuts.toArray(new XPRBcut[cuts.size()]));
//        }
        bprob.endCB();
        return 0;
    }

    public boolean isIntegerSolution() {
        boolean result = true;
        for (XPRBvar var : variables.values()) {
            if (Math.abs(var.getSol()) > 1e-10 && Math.abs(var.getSol() - 1) > 1e-10) {
                result = false;
            }
            if (!result)
                break;
        }
        return result;
    }

    @Override
    public void XPRSoptNodeEvent(XPRSprob oprob, Object o, IntHolder intHolder) {
        XPRBprob bprob = (XPRBprob) o;
        oprob.loadCuts(-1,-1);
        bprob.beginCB(oprob);
        bprob.sync(XPRB.XPRS_SOL);
        int num = oprob.getIntAttrib(XPRS.NODES);
        System.out.println("----------------------------------------------------------------------------------");
        System.out.println(String.format("XPRSoptNodeEvent  : treating optimal relaxation of node %4d", num));
        System.out.println(String.format("XPRSoptNodeEvent  : %4d  cuts loaded at node %4d", oprob.getIntAttrib(XPRS.CUTS), num));
        if (isIntegerSolution()) {
            System.out.println("integer solution");
            generateCut(bprob, oprob);
            if (cuts.size() > 1) {
                System.out.println(String.format("XPRSoptNodeEvent  : %4d cuts generated at node %4d", cuts.size(), num));
                XPRBcut[] cutadded = cuts.toArray(new XPRBcut[cuts.size()]);
                bprob.addCuts(cutadded);

                System.out.println(String.format("XPRSoptNodeEvent  : after addCuts %4d  cuts loaded at node %4d", oprob.getIntAttrib(XPRS.CUTS), num));
//                try {
//                    bprob.exportProb(String.format("%d_toto.lp", num));
//                } catch (IOException e) {
//                    e.printStackTrace();
//                }
//                int itype, int interp, int ncuts, const XPRScut mcutind[]
//                oprob.loadCuts();
                //                for (XPRBcut cut : cuts)
//                    cut.print();
            } else {
            }
        }
        bprob.endCB();
    }
}
