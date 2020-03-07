import java.awt.*;

import javax.swing.JButton;
import javax.swing.JFrame;

public class SnifferFrame extends JFrame {

	private static final long serialVersionUID = 1557630126395106359L;

	private TextPanel textPanel;
	private JButton btn;
	private Toolbar toolbar;

	public SnifferFrame(String title, Sniffer sniffer) {
		super(title);
		
		setLayout(new BorderLayout()); // set layout manager

        toolbar = new Toolbar();
        toolbar.setDataListener(() -> textPanel.getContent());

		textPanel = new TextPanel();
		btn = new JButton("Start");
		
		Thread cSniffer = new Thread(() ->  {
			try {
				textPanel.appendText("Starting Sniffer ...\n");
				sniffer.run();
			} catch (Exception e) {
				// TODO: handle exception
			}
		} );
	
		Thread textAreaUpate = new Thread(() -> {
			while (true) {
//				String[] buffer = sniffer.getPrintBuffer();
//				for (String string : buffer) {
//					System.out.println(string);
//					textArea.append(string);
//				};
//				textArea.append("Buffer size: " + buffer.length);
				System.out.println("Getting packages from c sniffer ...");
				Ipv4Header[] packs = sniffer
						.getPacketsFromBuffer(5);
				System.out.println("Buffer response size = " + packs.length);

//				textPanel.clear();
//				textPanel.appendText(sniffer.getStatistics().toString());

				for (Ipv4Header h : packs){
					if(h != null){
						textPanel.appendText(h.toString());
					}
				}

				try {
					Thread.sleep(500);
				} catch (Exception e){

				}
			}
		});
		
		btn.addActionListener(e -> {
			textPanel.appendText("Append data ...\n");
			if(!cSniffer.isAlive()) {
				cSniffer.start();
				textAreaUpate.start();
			}
		});

		add(toolbar, BorderLayout.NORTH);
		add(textPanel, BorderLayout.CENTER);
		add(btn, BorderLayout.SOUTH);
		
		setSize(800, 600);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setVisible(true);
		
	}
	
}
