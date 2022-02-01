import java.io.FileInputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MapTask implements Runnable {
    private final MapTaskArg task;
    private final List<MapTaskResult> mapTaskResults;

    public MapTask(MapTaskArg task, List<MapTaskResult> mapTaskResults) {
        this.task = task;
        this.mapTaskResults = mapTaskResults;
    }

    private String[] trimAndSplitWords(String words) {
        int start = 0;
        int end = (task.getDocumentOffset() == 0) ? task.getFragmentDimension() - 1 : task.getFragmentDimension();

        if (task.getDocumentOffset() != 0 && Character.isLetter(words.charAt(start)) && Character.isLetter(words.charAt(start + 1))) {
            while (Character.isLetter(words.charAt(start))) {
                start += 1;
            }
        } else if (task.getDocumentOffset() != 0) {
            start += 1;
        }

        if (Character.isLetter(words.charAt(end)) && Character.isLetter(words.charAt(end + 1))) {
            while (Character.isLetter(words.charAt(end))) {
                end += 1;
            }
        } else {
            end += 1;
        }
        
        String trimmedWords = (start > end) ? "" : words.substring(start, end);

        return trimmedWords.split("[^a-zA-Z0-9]+");
    }

    @Override
    public void run() {
        byte[] buffer = new byte[task.getFragmentDimension() + Constants.ADDITIONAL_FRAGMENT_READ_SIZE];
        try {
            FileInputStream fileReader = new FileInputStream(Constants.IN_RELATIVE_PATH + task.getDocumentName());
            if (task.getDocumentOffset() == 0) {
                fileReader.getChannel().position(task.getDocumentOffset());
            } else {
                fileReader.getChannel().position(task.getDocumentOffset() - 1);
            }

            int res = fileReader.read(buffer);
        } catch (IOException e) {
            e.printStackTrace();
        }

        int longestSize = 0;
        int longestSizeCount = 0;
        Map<Integer, Integer> occurrences = new HashMap<>();

        for (String str : trimAndSplitWords(new String(buffer))) {
            int wordLength = str.length();
            if (wordLength == 0) {
                continue;
            }

            occurrences.merge(wordLength, 1, Integer::sum);

            if (wordLength > longestSize) {
                longestSize = str.length();
                longestSizeCount = 1;
            } else if (wordLength == longestSize) {
                longestSizeCount += 1;
            }
        }

        mapTaskResults.add(new MapTaskResult(task.getDocumentName(), occurrences, longestSize, longestSizeCount));
    }
}
