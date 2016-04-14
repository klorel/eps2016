import javafx.util.Pair;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.LinkedList;

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
        TSPInstance instance = getInstance(AvailableInstance.att48);
        try {
            TSPXpressSolver solver = new TSPXpressSolver(instance);
            solver.buildProblem();
//            solver.problem.exportProb(XPRB.LP, instance.name + ".lp");
//            solver.
            solver.problem.mipOptimise();
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
