import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.FileWriter;
import java.io.IOException;

public class Toolbar extends JPanel implements ActionListener {

    private JButton saveButton;
    private JFileChooser jFileChooser;

    private DataListener dataListener;

    public Toolbar() {
        saveButton = new JButton("Save");
        setLayout(new FlowLayout(FlowLayout.LEFT));
        add(saveButton);
        jFileChooser = new JFileChooser();

        saveButton.addActionListener((ActionEvent actionEvent) -> {
            if(jFileChooser.showSaveDialog(this) == JFileChooser.APPROVE_OPTION){
                // Save file
                try(FileWriter fw = new FileWriter(jFileChooser.getSelectedFile())){
                    fw.write(dataListener.getData());
                } catch (IOException e) {
                    // TODO show error message
                    e.printStackTrace();
                }
            }
        });

    }

    public void setDataListener(DataListener dataListener) {
        this.dataListener = dataListener;
    }

    @Override
    public void actionPerformed(ActionEvent actionEvent) {

    }
}
