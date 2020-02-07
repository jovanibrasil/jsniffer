import java.awt.BorderLayout;

import javax.swing.JButton;
import javax.swing.JFrame;

public class SnifferFrame extends JFrame {

	private static final long serialVersionUID = 1557630126395106359L;

	private TextPanel textPanel;
	private JButton btn;

	public SnifferFrame(String title, Sniffer sniffer) {
		super(title);
		
		setLayout(new BorderLayout()); // set layout manager

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
				textPanel.clear();
				textPanel.appendText(sniffer.getStatistics().toString());
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
		
		add(textPanel, BorderLayout.CENTER);
		add(btn, BorderLayout.SOUTH);
		
		setSize(600, 500);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setVisible(true);
		
	}
	
}
