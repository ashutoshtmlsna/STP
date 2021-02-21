# include <iostream>
# include <vector>
# include <string>
# include <set>
# include <queue>
# include <utility>
# include <algorithm>
# include <fstream>

using namespace std;

//Class Declarations

//LAN
class lan{
	public:
		char id;
		int DP;
		vector<int> adj_bridges;
		vector<int> hosts;
		lan()
		{
			id = '\0';
			DP = -1;
		}
};

//Bridge
class bridge{
	public:
		int id;
		int root;
		int root_distance;
		vector<char> adj_lans;
		pair<char,int> RP;
		bridge()
		{
			id = -1;
			root = id;
			RP = make_pair('\0',-1);
			root_distance = -1;
		}
};

//Message
class message{
	public:
		int root;
		int dist;
		bridge source;
		int destination;
		char lan;
		message()
		{
			root = -1;
			dist = -1;
			destination = -1;
			lan = '\0';
		}
};

struct messageComparer
{
    bool operator()(message const &m1, message const &m2)
    {
    	if(m1.destination < m2.destination) return true;
    	else return false;
        //compare m1 and m2 and return true or false
    }
};

class traces{
	public:
		int time;
		int bridge;
		char status;
		message m;
};

//Functions to Print Bridge Network and LAN Network

void print_bridge_nw(vector<bridge> network)
{
	for(int i=0; i<network.size();i++)
	{
		cout<< "Bridge: " <<network[i].id<<endl;
		for(int j=0; j<network[i].adj_lans.size();j++)
			cout<<network[i].adj_lans[j]<<" ";
		cout<<endl;
	}
}

void print_lan_nw(vector<lan> network)
{
	for(int i=0; i<network.size();i++)
	{
		cout<< "LAN: "<<network[i].id<<endl;
		for(int j=0; j<network[i].adj_bridges.size();j++)
			cout<<network[i].adj_bridges[j]<<" ";
		cout<<endl;
		cout<<  "Designated Bridge: " <<network[i].DP<<endl;
	}
}

typedef set<message,messageComparer> msgSet;

//Forward the message from a bridge to its adjacent LANs
msgSet sendMsg(message m, vector<bridge> bridge_nw, vector<lan> lan_nw)
{
    msgSet msg;
    int root = m.root;
    int d = m.dist;
    bridge src = m.source;

    for (int i=0; i<bridge_nw.size(); i++)
    {
        if (src.id == bridge_nw[i].id)
        {
			for (int j=0; j<bridge_nw[i].adj_lans.size(); j++)
			{
				for (int k=0; k<lan_nw.size(); k++)
				{
					if (bridge_nw[i].adj_lans[j] == lan_nw[k].id)
					{
						for (int l=0; l<lan_nw[k].adj_bridges.size(); l++)
						{
							if (lan_nw[k].adj_bridges[l] != src.id)
							{
								message ms;
								ms.root = root;
								ms.dist = d;
								ms.source = src;
								ms.destination = lan_nw[k].adj_bridges[l];
								ms.lan = lan_nw[k].id;
                                msg.insert(ms);
							}
						}
					}
				}
			}

		}
    }

    return msg;

}

//Update Configuration of the bridge upon receipt of better message
message configUpdate(message m, vector<bridge> &bridge_nw)
{
	int root = m.root;
	int d = m.dist;
	bridge src = m.source;
	int destn = m.destination;
	char lan = m.lan;

	message rtn_msg;

	for (int i=0; i<bridge_nw.size(); i++)
	{
		if (destn == bridge_nw[i].id)
		{
			bridge b = bridge_nw[i];
			if (root < b.root)
			{
				rtn_msg.root = root;
				rtn_msg.dist = d+1;
				rtn_msg.source = bridge_nw[i];
				bridge_nw[i].root = root;
				bridge_nw[i].RP = make_pair(lan, src.id);
				bridge_nw[i].root_distance = d+1;
			}

			else if (root == b.root && d+1 < bridge_nw[i].root_distance)
			{
				rtn_msg.root = root;
				rtn_msg.dist = d+1;
				rtn_msg.source = bridge_nw[i];
				bridge_nw[i].root = root;
				bridge_nw[i].RP = make_pair(lan, src.id);
				bridge_nw[i].root_distance = d+1;
			}

			else if (root == b.root && d+1 <= bridge_nw[i].root_distance && src.id < bridge_nw[i].RP.second)
			{
				rtn_msg.root = root;
				rtn_msg.dist = d+1;
				rtn_msg.source = bridge_nw[i];
				bridge_nw[i].root = root;
				bridge_nw[i].RP = make_pair(lan, src.id);
				bridge_nw[i].root_distance = d+1;
			}

			else
			{
				rtn_msg.root = -1;
				rtn_msg.dist = d+1;
				rtn_msg.source = bridge_nw[i];
			}
		}
	}
	return rtn_msg;
}


int main(int argc, char *argv[])
{
    vector<bridge> sending_bridges;
    ifstream inputFile;

    //If lanfile name and/or random nodes to send message are not provided; load the default lan.txt
    if (argc < 2)
    {
		//Input the bridges from lan.txt from the folder itself
		cout<<"usage: "<< argv[0] <<" <filename>\n";
		inputFile.open("lan.txt");
    }

    //If filename is provided to load lan network
    else
    {
		//Input the bridges from (lanfilename).txt
		inputFile.open(argv[1]);
		if (!inputFile.is_open())
		{
			cout << "Could not open file. Check again.\n";
		}
		else
		{
			if (argc > 2) //if random nodes to initialize the network is provided as command line argument
			{
				for (int i = 2; i < argc; i++)
				{
					bridge b;

					b.id = atoi(argv[i]);
					b.root = b.id;

					sending_bridges.push_back(b);
				}
			}

		}
    }

    vector<bridge> bridge_nw;
    set<char> lan_set;
    string line;

    while (getline(inputFile, line))
    {
		bridge b;
		for (int i = 0; i <line.size(); i++)
		{
			if (i==0)
			{
				b.id = (int) line[i] - 48; //init its own id; typecasting as int
				b.root = b.id; //init itself as root
			}

			else
			{
				if (line[i] != ' ')//checking for white spaces
				{
					b.adj_lans.push_back(line[i]);
					lan_set.insert(line[i]);
				}
			}
		}
		sort(b.adj_lans.begin(), b.adj_lans.end());
		bridge_nw.push_back((b));
    }

    //Construct vectors of LANs
    vector<lan> lan_nw;
    while (!lan_set.empty())
    {
		char k = *lan_set.begin();
		lan l;
		l.id = k;
		for (int i = 0; i<bridge_nw.size(); i++)
		{
			for (int j = 0; j<bridge_nw[i].adj_lans.size(); j++)
			{
				if (bridge_nw[i].adj_lans[j] == k)
				{
					l.adj_bridges.push_back(bridge_nw[i].id);
				}
			}
		}
		lan_nw.push_back(l);
		lan_set.erase(lan_set.begin());
    }

    //print_lan_nw(lan_nw);

    //STP
    queue<message> generated, received;
    vector<traces> trace_queue;
    int ts = 0, init = 1;

    //If random nodes to initialize the network is not provided
    //All nodes send messages and converges to a final layout
    if (argc <= 2)
    {
		for (int i=0; i<bridge_nw.size(); i++)
		{
			message m;
			m.root = bridge_nw[i].id;
			m.dist = 0;
			m.source = bridge_nw[i];
			generated.push(m);
		}

		while (!generated.empty())
		{
		if(init != 1)
		{
			while (!generated.empty())
			{
				generated.pop();
			}
		}

		while (!received.empty())
		{
			message m = received.front();
			message to_publish = configUpdate(m, bridge_nw);
			if (to_publish.root != -1)
			{
				generated.push(to_publish);
			}
			traces tr;
			tr.time = ts;
            tr.bridge = m.destination;
            tr.status = 'r';
            tr.m = m;
            //cout << tr.time << " B" << tr.bridge << ":" << tr.status << " (B" << tr.m.root << " " << tr.m.dist << " B" << tr.m.source.id << ")\n";
            trace_queue.push_back(tr);
            received.pop();
		}

		queue<message> temp;
		while (!generated.empty())
		{
			message m = generated.front();
			msgSet receivedSet = sendMsg(m, bridge_nw, lan_nw);
			generated.pop();
			while (!receivedSet.empty())
			{
				received.push(*receivedSet.begin());
				receivedSet.erase(receivedSet.begin());
			}
			traces tr;
			tr.time = ts;
            tr.bridge = m.source.id;
            tr.status = 's';
            tr.m = m;
            //cout << tr.time << " B" << tr.bridge << ":" << tr.status << " (B" << tr.m.root << " " << tr.m.dist << " B" << tr.m.source.id << ")\n";
            trace_queue.push_back(tr);
            temp.push(m);
		}
		while (!temp.empty())
		{
			generated.push(temp.front());
			temp.pop();
		}
		ts++;
		//if (argc <= 2){ init = 0;}
		init = 0;
		}

		for(int i=0; i<lan_nw.size(); i++)
		{
			lan_nw[i].DP = *min_element(begin(lan_nw[i].adj_bridges),end(lan_nw[i].adj_bridges));

			int temp = bridge_nw[lan_nw[i].DP-1].root_distance;
			for (int j=0; j<lan_nw[i].adj_bridges.size(); j++)
			{
				int lan_dis = bridge_nw[lan_nw[i].adj_bridges[j]-1].root_distance;
				if (lan_dis < temp)
				{
					temp = lan_dis;
					lan_nw[i].DP = lan_nw[i].adj_bridges[j];
				}
			}
		}
    }

    else
    {
		for (int i=0; i<sending_bridges.size(); i++)
		{
			message m;
			m.root = sending_bridges[i].id;
			m.dist = 0;
			m.source = sending_bridges[i];
			if (init != 1)
			{
				m.root = bridge_nw[sending_bridges[i].id-1].root;
				m.dist = bridge_nw[sending_bridges[i].id-1].root_distance; // only in case of 3 problem due to +1
			}
			if (m.dist == -1){m.dist = 0;}

			//generated.push(m);
			msgSet receivedSet = sendMsg(m, bridge_nw, lan_nw);
			message to_publish = configUpdate (m, bridge_nw);
			while (!receivedSet.empty())
			{
				received.push(*receivedSet.begin());
				receivedSet.erase(receivedSet.begin());
			}
			traces tr;
			tr.time = ts;
            tr.bridge = m.source.id;
            tr.status = 's';
            tr.m = m;
            //cout << tr.time << " B" << tr.bridge << ":" << tr.status << " (B" << tr.m.root << " " << tr.m.dist << " B" << tr.m.source.id << ")\n";
            trace_queue.push_back(tr);
            ts++;


            while (!received.empty())
			{
			message m = received.front();
			message to_publish = configUpdate(m, bridge_nw);
			traces tr;
			tr.time = ts;
			tr.bridge = m.destination;
			tr.status = 'r';
			tr.m = m;
			//cout << tr.time << " B" << tr.bridge << ":" << tr.status << " (B" << tr.m.root << " " << tr.m.dist << " B" << tr.m.source.id << ")\n";
			trace_queue.push_back(tr);
			received.pop();
			}
			ts++;
			init = 0;
		}

		//Updating Designated Bridge for LANs based on distance and adjacent bridges id in that order
		for(int i=0; i<lan_nw.size(); i++)
		{
			lan_nw[i].DP = *min_element(begin(lan_nw[i].adj_bridges),end(lan_nw[i].adj_bridges));
			int temp = bridge_nw[lan_nw[i].DP-1].root_distance;
			for (int j=0; j<lan_nw[i].adj_bridges.size(); j++)
			{
				int lan_dis = bridge_nw[lan_nw[i].adj_bridges[j]-1].root_distance;
				if (lan_dis != -1 && lan_dis < temp)
				{
					temp = lan_dis;
					lan_nw[i].DP = lan_nw[i].adj_bridges[j];
				}
			}
		}
    } //else

	//cout<<"DP for lans"<<endl;
	//print_lan_nw(lan_nw);

	//Output as required
	for(int i=0; i<bridge_nw.size(); i++)
	{
		if (bridge_nw[i].root_distance == -1) {bridge_nw[i].root_distance = 0;bridge_nw[i].RP.second = bridge_nw[i].id;}
		cout<<"Bridge"<<bridge_nw[i].id<<": best configuration <" << bridge_nw[i].root << ", " << bridge_nw[i].root_distance << ">, from " << bridge_nw[i].RP.second << " via " << bridge_nw[i].RP.first << endl;
		for(int j=0; j<bridge_nw[i].adj_lans.size(); j++)
		{
			int flag=0;
			char c = bridge_nw[i].adj_lans[j];
			//cout<<" Port "<<c<<": <"<< bridge_nw[i].root << ", " ;
			if(c == bridge_nw[i].RP.first) {cout <<" Port "<<c<<": <"<< bridge_nw[i].root << ", " << bridge_nw[i].root_distance-1 << ", " << bridge_nw[i].RP.second << ">,     open " << "RP\n";flag=1;}
			for(int k=0;k<lan_nw.size();k++)
			{
				if(c==lan_nw[k].id && bridge_nw[i].id==lan_nw[k].DP && flag==0)
					{cout<<" Port "<<c<<": <"<< bridge_nw[i].root << ", " << bridge_nw[i].root_distance << ", " << bridge_nw[i].id << ">,     open " <<"DP\n";flag=1;break;}
			}
			if(flag==0)
			{
				for(int k=0;k<lan_nw.size();k++)
				{
					if(c==lan_nw[k].id)
						{cout<<" Port "<<c<<": <"<< bridge_nw[lan_nw[k].DP-1].root << ", " << bridge_nw[lan_nw[k].DP-1].root_distance << ", " << lan_nw[k].DP << ">,     closed " <<"NP\n";}
				}
				bridge_nw[i].adj_lans.erase(remove(bridge_nw[i].adj_lans.begin(), bridge_nw[i].adj_lans.end(), c), bridge_nw[i].adj_lans.end());
				j--;
			}
		}
		cout<<endl;
	}

	//Updating Lan network

	for(int i=0; i<lan_nw.size();i++)
	{
		char c = lan_nw[i].id;
		vector<int> l;
		for(int i=0; i<bridge_nw.size();i++)
			for(int j=0; j<bridge_nw[i].adj_lans.size(); j++)
				if (bridge_nw[i].adj_lans[j] == c) l.push_back(bridge_nw[i].id);
		lan_nw[i].adj_bridges = l;
	}

    cout<< "Final LAN Network:" <<endl;
	print_lan_nw(lan_nw);
	cout<<"\nFinal Bridge Network:" <<endl;
	print_bridge_nw(bridge_nw);

    return 0;
}

















