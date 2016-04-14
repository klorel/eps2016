import com.dashoptimization.*;
import javafx.util.Pair;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.LinkedHashMap;

/**
 * Created by mruiz on 13/04/2016.
 */
public class TSPInstance {
    public LinkedHashMap<Integer, Pair<Double, Double>> coordinates = new LinkedHashMap<>();
    public double[][] distance;
    public int[][] x;
    public int nVar;
    public int nPoints;
    public String name;

    public TSPInstance(String instance) {
        this.name = instance;
    }


    void buildDistance() {
        nPoints = coordinates.size();
        distance = new double[nPoints][nPoints];
        for (int i = 0; i < nPoints; ++i) {
            for (int j = 0; j < nPoints; ++j) {
                distance[i][j] = Math.sqrt(Math.pow(coordinates.get(i + 1).getKey() - coordinates.get(j + 1).getKey(), 2.0) + Math.pow(coordinates.get(i + 1).getValue() - coordinates.get(j + 1).getValue(), 2.0));
            }
        }

        x = new int[nPoints][nPoints];
        nVar = 0;
        for (int i = 0; i < nPoints; ++i) {
            for (int j = i + 1; j < nPoints; ++j, ++nVar) {
                x[i][j] = nVar;
                x[j][i] = nVar;
            }
        }
    }

}
