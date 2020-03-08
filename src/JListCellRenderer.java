import javax.swing.*;
import java.awt.*;

public class JListCellRenderer extends JLabel implements ListCellRenderer<Ipv4Header> {

    public JListCellRenderer() {
        setOpaque(true);
    }

    @Override
    public Component getListCellRendererComponent(JList<? extends Ipv4Header> list, Ipv4Header ipv4Header,
        int index, boolean isSelected, boolean cellHasFocus) {

        setText("Received packet: " + String.valueOf(ipv4Header.totalLength));

        if (isSelected) {
            setBackground(list.getSelectionBackground());
            setForeground(list.getSelectionForeground());
        } else {
            setBackground(list.getBackground());
            setForeground(list.getForeground());
        }

        return this;
    }
}
