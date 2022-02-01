public class MapTaskArg {
    private final String documentName;
    private final int documentOffset;
    private final int fragmentDimension;

    public MapTaskArg(String documentName, int documentOffset, int fragmentDimension) {
        this.documentName = documentName;
        this.documentOffset = documentOffset;
        this.fragmentDimension = fragmentDimension;
    }

    public String getDocumentName() {
        return documentName;
    }

    public int getDocumentOffset() {
        return documentOffset;
    }

    public int getFragmentDimension() {
        return fragmentDimension;
    }
}
