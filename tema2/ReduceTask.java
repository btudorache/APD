import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ReduceTask extends Thread {
    public final ReduceTaskArg reduceTaskArg;
    public final List<ReduceTaskResult> reduceTaskResults;

    public ReduceTask(ReduceTaskArg reduceTaskArg, List<ReduceTaskResult> reduceTaskResults) {
        this.reduceTaskArg = reduceTaskArg;
        this.reduceTaskResults = reduceTaskResults;
    }

    @Override
    public void run() {
        List<MapTaskResult> partialMapTaskResults = reduceTaskArg.getPartialResults();

        Map<Integer, Integer> finalResults = new HashMap<>();
        int longestSize = 0;
        int longestSizeOccurrences = 0;
        int numWords = 0;

        for (MapTaskResult mapTaskResult : partialMapTaskResults) {
            Map<Integer, Integer> partialOcc = mapTaskResult.getWordOccurrences();
            for (Integer key : partialOcc.keySet()) {
                finalResults.merge(key, partialOcc.get(key), Integer::sum);
                numWords += partialOcc.get(key);
            }

            if (mapTaskResult.getLongestSize() > longestSize) {
                longestSize = mapTaskResult.getLongestSize();
                longestSizeOccurrences = mapTaskResult.getLongestSizeAppearances();
            } else if (mapTaskResult.getLongestSize() == longestSize) {
                longestSizeOccurrences += mapTaskResult.getLongestSizeAppearances();
            }
        }

        double fileRank = 0;
        for (Integer key : finalResults.keySet()) {
            fileRank += Constants.FIBONACCI_INDEXES.get(key - 1) * finalResults.get(key);
        }
        fileRank /= numWords;

        reduceTaskResults.add(new ReduceTaskResult(reduceTaskArg.getDocumentName(), longestSize, longestSizeOccurrences, fileRank));
    }
}
