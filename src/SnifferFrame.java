import java.awt.BorderLayout;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JTextArea;

public class SnifferFrame extends JFrame {

	private static final long serialVersionUID = 1557630126395106359L;

	private JTextArea textArea;
	private JButton btn;

	public SnifferFrame(String title, Sniffer sniffer) {
		super(title);
		
		setLayout(new BorderLayout()); // set layout manager
	
		textArea = new JTextArea();
		btn = new JButton("Start");
		
		Thread cSniffer = new Thread(() ->  {
			try {
				textArea.append("Starting Sniffer ...\n");
				sniffer.run();
			} catch (Exception e) {
				// TODO: handle exception
			}
		} );
	
		Thread textAreaUpate = new Thread(() -> {
			while (true) {
				String[] buffer = sniffer.getPrintBuffer();
				for (String string : buffer) {
					System.out.println(string);
					textArea.append(string);
				};
				textArea.append("Buffer size: " + buffer.length);
			}
		});
		
		btn.addActionListener(e -> {
			textArea.append("Append data ...\n");
			if(!cSniffer.isAlive()) {
				cSniffer.start();
				textAreaUpate.start();
			}
		});
		
		add(textArea, BorderLayout.CENTER);
		add(btn, BorderLayout.SOUTH);
		
		setSize(600, 500);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setVisible(true);
		
	}
	
}
