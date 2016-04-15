import com.dashoptimization.XPRB;
import com.dashoptimization.XPRBprob;
import com.dashoptimization.XPRS;
import com.dashoptimization.XPRSprob;
import javafx.util.Pair;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

/**
 * Created by mruiz on 13/04/2016.
 */
public class TSPExample {

    public static TSPInstance getInstance(AvailableInstance availableInstance) throws IOException {
        TSPInstance result = new TSPInstance(availableInstance.toString());
        BufferedReader br = new BufferedReader(new FileReader(availableInstance.toString() + ".txt"));
        try {
            StringBuilder sb = new StringBuilder();
            String line = br.readLine();
            while (line != null) {
                String[] buffer = line.split(" ");
                result.coordinates.put(Integer.parseInt(buffer[0]), new Pair<Double, Double>(Double.parseDouble(buffer[1]), Double.parseDouble(buffer[2])));
                line = br.readLine();
            }
            result.buildDistance();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            br.close();
        }
        return result;
    }

    public static void main(String[] args) throws IOException {
        TSPInstance instance = getInstance(AvailableInstance.dj38);
        try {
            TSPXpressSolver solver = new TSPXpressSolver(instance);
            solver.buildProblem();
//            solver.problem.exportProb(XPRB.LP, instance.name + ".lp");
//            solver.
            XPRBprob bprob = solver.problem;
            XPRSprob oprob = bprob.getXPRSprob();

            bprob.setCutMode(1);

            oprob.addMessageListener(solver);
            oprob.addPreIntsolListener(solver, bprob);
            oprob.addCutMgrListener(solver, bprob);
            oprob.addOptNodeListener(solver, bprob);

            bprob.setMsgLevel(3);

            oprob.setIntControl(XPRS.THREADS, 1);
            oprob.setIntControl(XPRS.ROOTPRESOLVE, 0);
            oprob.setIntControl(XPRS.OUTPUTLOG, XPRS.OUTPUTLOG_FULL_OUTPUT);


            oprob.setIntControl(XPRS.HEURSTRATEGY, 0);
            oprob.setIntControl(XPRS.CUTSTRATEGY, 0);

            bprob.setSense(XPRB.MINIM);
            bprob.mipOptimise();
//            LinkedList<LinkedList<Integer>> sub_tours = solver.getSubtour();
//            solver.breakSubTour(sub_tours);
//            solver.problem.mipOptimise();
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }
    }


    public enum AvailableInstance {
        dj38,
        att48,
        eil101,
        d1291
    }

}
