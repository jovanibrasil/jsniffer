import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

public class CustomJList extends JList<EthernetHeader> {

    private DetailsListener detailsListener;

    public CustomJList(ListModel<EthernetHeader> listModel) {
        super(listModel);
        setMaximumSize(new Dimension(1280, 600));

        addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent mouseEvent) {
                JList list = (JList) mouseEvent.getSource();
                if (mouseEvent.getClickCount() == 2) {
                    int index = list.locationToIndex(mouseEvent.getPoint());
                    detailsListener.setDetails(listModel.getElementAt(index).toString());
                }
            }
        });

    }

    public void setDetailsListener(DetailsListener detailsListener) {
        this.detailsListener = detailsListener;
    }
}
