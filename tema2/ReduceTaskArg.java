import java.util.List;

public class ReduceTaskArg {
    public final List<MapTaskResult> partialResults;
    public final String documentName;

    public ReduceTaskArg(List<MapTaskResult> partialResults, String documentName) {
        this.partialResults = partialResults;
        this.documentName = documentName;
    }

    public List<MapTaskResult> getPartialResults() {
        return partialResults;
    }

    public String getDocumentName() {
        return documentName;
    }
}
