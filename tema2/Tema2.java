import java.io.*;
import java.util.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;

public class Tema2 {
    public static void main(String[] args) throws InterruptedException {
        if (args.length < 3) {
            System.err.println("Usage: Tema2 <workers> <in_file> <out_file>");
            return;
        }

        int numWorkers = Integer.parseInt(args[0]);
        File inFile = new File(args[1]);

        int fragment_dim = 0;
        int numFiles = 0;
        List<File> files = new ArrayList<>();

        try (BufferedReader br = new BufferedReader(new FileReader(inFile))) {
            fragment_dim = Integer.parseInt(br.readLine());
            numFiles = Integer.parseInt(br.readLine());

            for (int i = 0; i < numFiles; i++) {
                String fileName = br.readLine();
                files.add(new File(fileName));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        // build and run map tasks
        ExecutorService pool = Executors.newFixedThreadPool(numWorkers);
        List<MapTaskResult> syncMapTaskResultList = Collections.synchronizedList(new ArrayList<>());

        for (File file : files) {
            int fileLength = (int) file.length();
            int fileFragments =  fileLength / fragment_dim;

            for (int i = 0; i < fileFragments; i++) {
                MapTaskArg taskArg = new MapTaskArg(file.getName(), i * fragment_dim, fragment_dim);
                pool.submit(new MapTask(taskArg, syncMapTaskResultList));
            }

            if (fileLength % fragment_dim != 0) {
                MapTaskArg taskArg = new MapTaskArg(file.getName(), fileFragments * fragment_dim, fileLength % fragment_dim);
                pool.submit(new MapTask(taskArg, syncMapTaskResultList));
            }
        }

        pool.shutdown();
        boolean tasksResult = pool.awaitTermination(60, TimeUnit.SECONDS);

        // restructure reduce tasks
        Map<String, ArrayList<MapTaskResult>> structuredMapResults = new HashMap<>();
        for (MapTaskResult mapTaskResult : syncMapTaskResultList) {
            String fileName = mapTaskResult.getDocumentName();

             if (!structuredMapResults.containsKey(fileName)) {
                 structuredMapResults.put(fileName, new ArrayList<>(Collections.singletonList(mapTaskResult)));
             } else {
                 structuredMapResults.get(fileName).add(mapTaskResult);
             }
        }

        // build and run reduce tasks
        List<ReduceTaskResult> syncReduceTaskResultList = Collections.synchronizedList(new ArrayList<>());
        pool = Executors.newFixedThreadPool(numWorkers);

        for (String fileName : structuredMapResults.keySet()) {
            ReduceTaskArg taskArg = new ReduceTaskArg(structuredMapResults.get(fileName), fileName);
            pool.submit(new ReduceTask(taskArg, syncReduceTaskResultList));
        }

        pool.shutdown();
        tasksResult = pool.awaitTermination(60, TimeUnit.SECONDS);


        // sort by file rank descending
        List<ReduceTaskResult> sortedList = syncReduceTaskResultList
                .stream()
                .sorted(Collections.reverseOrder(Comparator.comparingDouble(ReduceTaskResult::getFileRank)))
                .collect(Collectors.toList());

        // write results to output
        try (PrintWriter pw = new PrintWriter(new FileWriter(args[2]))) {
            for (ReduceTaskResult result : sortedList) {
                String res = result.getDocumentName() + "," +
                             String.format("%.2f", result.getFileRank()) + "," +
                             result.getLongestSize() + "," +
                             result.getLongestSizeOccurrences();
                pw.println(res);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
