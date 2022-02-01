public class ReduceTaskResult {
    private final String documentName;
    private final int longestSize;
    private final int longestSizeOccurrences;
    private final double fileRank;

    public ReduceTaskResult(String documentName, int longestSize, int longestSizeOccurrences, double fileRank) {
        this.documentName = documentName;
        this.longestSize = longestSize;
        this.longestSizeOccurrences = longestSizeOccurrences;
        this.fileRank = fileRank;
    }

    public String getDocumentName() {
        return documentName;
    }

    public int getLongestSize() {
        return longestSize;
    }

    public int getLongestSizeOccurrences() {
        return longestSizeOccurrences;
    }

    public double getFileRank() {
        return fileRank;
    }
}
