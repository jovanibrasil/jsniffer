import javax.swing.*;
import java.awt.*;

public class JListCellRenderer extends JLabel implements ListCellRenderer<EthernetHeader> {

    public JListCellRenderer() {
        setOpaque(true);
    }

    @Override
    public Component getListCellRendererComponent(JList<? extends EthernetHeader> list, EthernetHeader ethernetHeader,
        int index, boolean isSelected, boolean cellHasFocus) {

        setText(ethernetHeader.destMac + "  " + ethernetHeader.sourceMac + "  " + String.format("0x%04x", ethernetHeader.etherType));

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
