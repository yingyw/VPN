import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.security.NoSuchAlgorithmException;
import java.security.spec.InvalidKeySpecException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicInteger;

import javax.crypto.SecretKey;


public class VPNConnection implements Runnable {	
	private static final int MAGICNUMBER_HANDSHAKE1 = 0xBEBEBEBE;
	private static final int MAGICNUMBER_HANDSHAKE2 = 0x12345678;
	private static final int MAGICNUMBER_CLIENTTOSERVER = 0x23456564;
	private static final int MAGICNUMBER_SERVERTOCLIENT = 0xF822A294;
	private static final int PRINT_MESSAGE_INTERVAL = 2000;
	
	private static AtomicInteger currentClientId;
	
	private static final byte OPEN_BYTE = 1;
	private static final byte CLOSE_BYTE = 2;
	private static final byte SEND_DATA_BYTE = 3;
	
	
	
		private Socket clientSocket;
		private InputStream clientInputStream;
		private OutputStream clientOutputStream;
		private byte[] ClientRSAkey;
		private EncryptionUtils encrypt_decrypt;
		
		private HashMap<Integer, Socket> connectionMap;
		
		private SecretKey AESKey;
		
		private int clientId;
		
		private long lastTimeSendingPackets = -1;
		private int totalBitSent = 0;
		
		public VPNConnection(
				Socket socket, 
				ConcurrentLinkedQueue<Integer> connectionQueue, 
				ConcurrentHashMap<Integer, Integer> validConnection,
				SecretKey AESKey,
				AtomicInteger currentClientId) 
						throws InvalidKeySpecException, NoSuchAlgorithmException {
			this.AESKey = AESKey;
			this.currentClientId = currentClientId;
			this.clientSocket = socket;
			this.connectionMap = new HashMap<Integer, Socket>();
			try {
				clientInputStream = socket.getInputStream();
				clientOutputStream = socket.getOutputStream();
				
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		private void forwardMessageAndResponse() {
			//System.out.println("\n entering dorwardMessage");
			
			
			byte[] data = ConnectionUtils.readFromClient(clientInputStream);
			
			//System.out.println("Time for receiving message from the client is " + System.currentTimeMillis());

			
			
			byte[] encryptedMessage = ConnectionUtils.verifyAndGetData(data, data.length, MAGICNUMBER_CLIENTTOSERVER);
			
//			System.out.println("data received from client ");
//
//			for (int i = 0; i < encryptedMessage.length; i++)
//				System.out.print(" " + encryptedMessage[i]);

			//TODO : change this to address, metadata and data
			byte[] fullMessage = encrypt_decrypt.decryptByClientAESKey(encryptedMessage);// decrypt()
			
//			System.out.println("\n data of full message");
//			for (int i = 0; i < fullMessage.length; i++)
//				System.out.print(" " + (int)fullMessage[i]);

			
			byte[] message = new byte[fullMessage.length - 5];
			
			System.arraycopy(fullMessage, 5, message, 0, fullMessage.length - 5);
			
			//System.out.println("message length " + message.length);
			
			ByteBuffer buffer = ByteBuffer.wrap(fullMessage).order(ByteOrder.LITTLE_ENDIAN);
			byte type = buffer.get();
			int connectionId = buffer.getInt();
			
			String typeString = "";
			typeString = type == 1 ? "open" : "close";
			typeString = type == 3 ? "send data " : typeString;
			System.out.println("Received type " + typeString + " for client id " + clientId);
			
			if (type == OPEN_BYTE) {
				byte[] address = new byte[4];
				buffer.get(address);
				InetAddress addr = null;
				try {
					addr = InetAddress.getByAddress(address);
				} catch (UnknownHostException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
				byte[] portBytes = new byte[2];
				buffer.get(portBytes);
				int port = ((portBytes[0] & 0xff) << 8) | (portBytes[1] & 0xff);
				
//				System.out.println("addr is " + addr);
//				System.out.println("port is " + port);
				
				
				
				try {
					//System.out.println("Socket is already initialized for connection " + connectionId);
					Socket serverSocket = new Socket(addr, port);
					serverSocket.setSoTimeout(100);
					connectionMap.put(connectionId, serverSocket);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					byte[] encrypted = encrypt_decrypt.encryptByClientAESKey((ByteBuffer.allocate(5).order(ByteOrder.LITTLE_ENDIAN)
							 .put(CLOSE_BYTE).putInt(connectionId).array()));
							int length = 8 + encrypted.length;
							try {
								clientSocket.getOutputStream().write(
												 ByteBuffer.allocate(length).order(ByteOrder.LITTLE_ENDIAN).putInt(length)
												 .putInt(MAGICNUMBER_SERVERTOCLIENT)
												 .put(encrypted).array());
								clientSocket.getOutputStream().flush();
							} catch (IOException e2) {
								System.out.println("Error occured while writing to client " + clientId + ", connection closed");
								try {
									clientSocket.close();
								} catch (IOException e1) {
									return;
								}
							}
							
					return;
				}
				
//				System.out.println("received open from the client");
//				System.out.println("address is " + addr);
//				System.out.println("port is " + port);

			} else if (type == CLOSE_BYTE) {
				//TODO : add things to close \
				try {
				connectionMap.get(connectionId).close();
				} catch (Exception e) {}
				
			} else {
				// should be send data 
				try {

					System.out.println("\n\nmessage length from the client " + message.length);
//					System.out.println("start of message");
//			        System.out.println(new String(message, "UTF-8").substring(0, Math.min(message.length, 30)));
//			        System.out.println("end of message\n\n");
//
//					System.out.println("Time for starting request on the server is " + System.currentTimeMillis());
//
//					System.out.println("the connection id " + connectionId);
//					System.out.println("socketStream " + connectionMap.get(connectionId));
//					System.out.println("outputStream" + connectionMap.get(connectionId).getOutputStream());
					
					if (connectionMap.containsKey(connectionId)) {
						
					connectionMap.get(connectionId).getOutputStream().write(message); // write the decrypted message from client to the server
					connectionMap.get(connectionId).getOutputStream().flush();
					
					//ReceiveMessageFromServerAndForwardToClient(connectionId);
					}
					

					

				} catch (IOException e) {
					e.printStackTrace();
					System.out.println("Error occured while writing to client " + clientId + ", connection closed");
					try {
						closeConnectionToServer(connectionId);
						clientSocket.close();
					} catch (IOException e1) {
						return;
					}
					
					return;
				}
			}			
			
		}
		
		private void closeConnectionToServer(int connectionId) {
			byte[] encrypted = encrypt_decrypt.encryptByClientAESKey((ByteBuffer.allocate(5).order(ByteOrder.LITTLE_ENDIAN)
			 .put(CLOSE_BYTE).putInt(connectionId).array()));
			int length = 8 + encrypted.length;
			try {
				clientSocket.getOutputStream().write(
								 ByteBuffer.allocate(length).order(ByteOrder.LITTLE_ENDIAN).putInt(length)
								 .putInt(MAGICNUMBER_SERVERTOCLIENT)
								 .put(encrypted).array());
				clientSocket.getOutputStream().flush();
			} catch (IOException e) {
				
			}
			
			try {
				if (!connectionMap.get(connectionId).isClosed())
				connectionMap.get(connectionId).close();
			} catch (Exception e) {}
			
			connectionMap.remove(connectionId);
		}
		
		private void ReceiveMessageFromServerAndForwardToClient(int connectionId) throws IOException {
			byte[] serverResponse = new byte[0];
			
			byte[] buf = new byte[16000];					    
			int read = 0;
			
			try {
				read = connectionMap.get(connectionId).getInputStream().read(buf);
				if (read < 0) {
					closeConnectionToServer(connectionId);
				} else {
				
					if (read == 0) 
						return;
					
	//				System.out.println("\n message received from the server " + read);
					
//					byte[] partMessage = new byte[Math.min(30, read)];
//					for (int i = 0; i < partMessage.length; i++) {
//						partMessage[i] = buf[i];
//					}
//					System.out.println(partMessage + "\n");
					
					encryptAndMessageToClient(ByteBuffer.allocate(read).put(buf, 0, read).array(), SEND_DATA_BYTE, connectionId);
				}
			} catch (SocketTimeoutException se) {
				
			} catch (IOException e) {
				closeConnectionToServer(connectionId);

			}
//	        while ((read = connectionMap.get(connectionId).getInputStream().read(buf)) != -1) {
//	        	System.out.println("read " + read + " bytes");
//		        //System.out.println(new String(buf, "UTF-8"));
//
//	        	byte[] tempServerResponse = new byte[serverResponse.length + read];
//	        	System.arraycopy(serverResponse, 0, tempServerResponse, 0, serverResponse.length);
//	        	System.arraycopy(buf, 0, tempServerResponse, serverResponse.length, read);
//	        	serverResponse = tempServerResponse;
//	        	if (read < 4096)
//	        		break;
//	        }
//	        
//	    
//	        String serverResponseString = new String(serverResponse, "UTF-8");
//	        System.out.println("serverResponse " + serverResponseString.substring(0, Math.min(30, serverResponseString.length())));
//
//			System.out.println("Time for end getting message from the server is " + System.currentTimeMillis());
//
//			encryptAndMessageToClient(serverResponse, SEND_DATA_BYTE, connectionId);
	        
		}
		
		private void encryptAndMessageToClient(byte[] message, byte dataType, int connectionId) {
			int preEncryptedLength = 1 + 4 + message.length;
	        
	        ByteBuffer unencryptedBuffer = ByteBuffer.allocate(preEncryptedLength).order(ByteOrder.LITTLE_ENDIAN);
	        
	        unencryptedBuffer.put(dataType);
	        unencryptedBuffer.putInt(connectionId);
	        unencryptedBuffer.put(message);
	        
	        byte[] encryptedResponse = encrypt_decrypt.encryptByClientAESKey(unencryptedBuffer.array());

	        int length = 4 + 4 + encryptedResponse.length; // length + magic number + encrypted part
	        
	        
	        
	        
	        
	        ByteBuffer responseBuffer = ByteBuffer.allocate(length).order(ByteOrder.LITTLE_ENDIAN);
	        responseBuffer.putInt(length);
	        responseBuffer.putInt(MAGICNUMBER_SERVERTOCLIENT);
	        
	        responseBuffer.put(encryptedResponse);
	        
	        byte[] wrappedResponseFromServer = responseBuffer.array();
	        
	        //System.out.println("\n\nthe encrypted message length " + length);
	        
	        
//	        System.out.println("The original message bytes are " + message.length + " bits");
//	        
//	        for (int i = 0; i < Math.min(30, message.length); i++)
//	        	System.out.printf("%02X ", message[i]);
//	        
//	        System.out.println("\nThe encrytpted message bytes are");
//	        
//	        for (int i = 0; i < Math.min(30, encryptedResponse.length); i++)
//	        	System.out.printf("%02X ", encryptedResponse[i]);
	        if (System.currentTimeMillis() - lastTimeSendingPackets >= PRINT_MESSAGE_INTERVAL)
	        {
	        	totalBitSent += wrappedResponseFromServer.length;
	        	System.out.println(totalBitSent + " bits are sent back to the client " + clientId);
	        	lastTimeSendingPackets = System.currentTimeMillis();
	        }
//	        for (int i = 0; i < 4; i++)
//	        	System.out.printf("%02X ", wrappedResponseFromServer[i]);
//	        System.out.println();
	        
	        try {
				clientOutputStream.write(wrappedResponseFromServer);
				clientOutputStream.flush();	
			} catch (IOException e) {
				try {
					connectionMap.get(connectionId).close();
				} catch (Exception ec) {}
				connectionMap.remove(connectionId);
				return;
				
			}
	        
	        
			//System.out.println("Time for end writting message to the client is " + System.currentTimeMillis());
		}
		
		public void waitConnectionFromClient() {
			
			
			long startTime = System.currentTimeMillis();
			
			while (true) {
				if (clientSocket.isClosed()) {
					return;
					// the connection is closed due to an error
				}
				try {
					Set<Integer> idsToCheck = new HashSet<Integer>(connectionMap.keySet());
					for (Integer connectionId : idsToCheck) {
//						 Socket serverSocket = connectionMap.get(connectionId);
//						 DataInputStream inputStream = serverSocket.getInputStream();
						 
						 //System.out.println("Checking the connection for " + connectionId);
						 if (connectionMap.get(connectionId).isClosed()) {
							 
							 closeConnectionToServer(connectionId);
							 continue;
						 }
						 //if (clientSocket.getInputStream().available() != 0) {
							 
							 //System.out.println("Bytes available for connection is " + clientSocket.getInputStream().available());
							 ReceiveMessageFromServerAndForwardToClient(connectionId);
							 //continue;
						 //}						 
					}
					
					if (clientInputStream.available() == 0) {
						Thread.sleep(10);
					} else {
						forwardMessageAndResponse();
					}
				} catch (IOException e) {
					System.out.println("waiting connection error io exception");
					e.printStackTrace();
				} catch (InterruptedException e) {
					System.out.println("interrptedException in waitConnection");
					e.printStackTrace();
				}
			}
		}
		
		
		private void connectWithClient() throws InvalidKeySpecException, NoSuchAlgorithmException {
			System.out.println("A new Connection received for client id " + clientId);
			byte[] data = ConnectionUtils.readFromClient(clientInputStream); // first handshake
			byte[] keyC = ConnectionUtils.verifyAndGetData(data, data.length, MAGICNUMBER_HANDSHAKE1);
			//System.out.println("KeyC length: " + keyC.length);
			ClientRSAkey = keyC;
			encrypt_decrypt = new EncryptionUtils(AESKey, ClientRSAkey);
			// fixed 4 byte (length of the remaining items)
			// KEY_SIZE bytes (keyS randomly generated by server)
			// client id token (not added currently)
			byte[] key = encrypt_decrypt.getServerKey();

			//System.out.println("handshake2 total length " + (key.length + 4 + 4));
			ByteBuffer buffer = ByteBuffer.allocate(4 + 4 + key.length).order(ByteOrder.LITTLE_ENDIAN);
			buffer.putInt(key.length + 4 + 4);
			buffer.putInt(MAGICNUMBER_HANDSHAKE2);
						
			
			//System.out.println(key);
			buffer.put(key);


			byte[] byteArray = buffer.array();
			
			//System.out.println("the total key sent to the client: " + key.length + 8 + " length");
//			for (int i = 0; i < Math.min(100, byteArray.length); i++)
//				System.out.print(String.format("%02X ", byteArray[i]));
			try {
				clientOutputStream.write(buffer.array(), 0, byteArray.length);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		
		
		
		@Override
		public void run() {
			clientId = currentClientId.getAndIncrement();
			try {
				connectWithClient();
			} catch (InvalidKeySpecException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (NoSuchAlgorithmException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			waitConnectionFromClient();
			
		}
		
	}
	
	
