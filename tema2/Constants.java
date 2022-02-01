import java.util.ArrayList;
import java.util.List;

public class Constants {
    public static String IN_RELATIVE_PATH = "tests/files/";
    public static int ADDITIONAL_FRAGMENT_READ_SIZE = 15;
    public static int FIBONACCI_ARRAY_LENGTH = 20;
    public static List<Integer> FIBONACCI_INDEXES = new ArrayList<>();

    static {
        int firstElem = 0;
        int secondElem = 1;
        for (int i = 0; i < FIBONACCI_ARRAY_LENGTH; i++) {
            int newElem = firstElem + secondElem;
            FIBONACCI_INDEXES.add(newElem);

            firstElem = secondElem;
            secondElem = newElem;
        }
    }
}
