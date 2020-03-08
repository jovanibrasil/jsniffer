import java.awt.*;

import javax.swing.*;

public class SnifferFrame extends JFrame {

	private static final long serialVersionUID = 1557630126395106359L;

	private TextPanel logPanel;
	private Toolbar toolbar;
	private DetailsPanel detailsPanel;
	private CustomJList list;

	public SnifferFrame(String title, Sniffer sniffer) {
		super(title);
		
		setLayout(new BorderLayout()); // set layout manager

        toolbar = new Toolbar();
        toolbar.setDataListener(() -> logPanel.getContent());

		DefaultListModel<EthernetHeader> ethernetHeaderDefaultListModel = new DefaultListModel<>();
		list = new CustomJList(ethernetHeaderDefaultListModel);
		list.setCellRenderer(new JListCellRenderer());
		list.setDetailsListener(text -> detailsPanel.setText(text));

		logPanel = new TextPanel();
		detailsPanel = new DetailsPanel();

		Thread cSniffer = new Thread(() ->  {
			try {
				logPanel.appendText("Starting Sniffer ...\n");
				sniffer.run();
			} catch (Exception e) {
				// TODO: handle exception
			}
		} );
		Thread textAreaUpdate = new Thread(() -> {
			while (true) {
//				String[] buffer = sniffer.getPrintBuffer();
//				for (String string : buffer) {
//					System.out.println(string);
//					textArea.append(string);
//				};
//				textArea.append("Buffer size: " + buffer.length);
				System.out.println("Getting packages from c sniffer ...");
				EthernetHeader[] packs = sniffer
						.getPacketsFromBuffer(5);
				System.out.println("Buffer response size = " + packs.length);

//				textPanel.clear();
//				textPanel.appendText(sniffer.getStatistics().toString());

				for (EthernetHeader h : packs){
					if(h != null){
						ethernetHeaderDefaultListModel.addElement(h);
					}
				}

				try {
					Thread.sleep(500);
				} catch (Exception e){

				}
			}
		});
		toolbar.setSnifferListener(new SnifferListener() {
			@Override
			public void start() {
				logPanel.appendText("Append data ...\n");
				if(!cSniffer.isAlive()) {
					cSniffer.start();
					textAreaUpdate.start();
				}
			}

			@Override
			public void stop() {

			}
		});

		add(toolbar, BorderLayout.NORTH);
		add(detailsPanel, BorderLayout.EAST);
		add(new JScrollPane(list), BorderLayout.CENTER);
		add(logPanel, BorderLayout.SOUTH);
		
		setSize(1280, 720);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setVisible(true);
		
	}
	
}
