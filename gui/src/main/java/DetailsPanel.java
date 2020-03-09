import javax.swing.*;
import java.awt.*;

public class DetailsPanel extends JPanel {

    private JTextArea textArea;

    public DetailsPanel() {
        this.textArea = new JTextArea();

        setLayout(new BorderLayout());
        add(new JScrollPane(textArea), BorderLayout.CENTER);
        setPreferredSize(new Dimension(400, 600));
    }

    public void appendText(String text){
        textArea.append(text);
    }

    public void clear() {
        textArea.setText("");
    }

    public String getContent() {
        return textArea.getText();
    }

    public void setText(String text) {
        this.textArea.setText(text);
    }

}