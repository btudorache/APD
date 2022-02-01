import java.util.Map;

public class MapTaskResult {
    private final String documentName;
    private final Map<Integer, Integer> wordOccurrences;
    private final int longestSize;
    private final int longestSizeAppearances;

    public MapTaskResult(String documentName, Map<Integer, Integer> wordOccurrences, int longestSize, int longestSizeAppearances) {
        this.documentName = documentName;
        this.wordOccurrences = wordOccurrences;
        this.longestSize = longestSize;
        this.longestSizeAppearances = longestSizeAppearances;
    }

    public String getDocumentName() {
        return documentName;
    }

    public Map<Integer, Integer> getWordOccurrences() {
        return wordOccurrences;
    }

    public int getLongestSize() {
        return longestSize;
    }

    public int getLongestSizeAppearances() {
        return longestSizeAppearances;
    }
}
