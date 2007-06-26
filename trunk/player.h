#ifndef PLAYER_H
#define PLAYER_H

class Player {
	enum PlayerType {Local,Remote,Computer};

	Player(PlayerType t);
	~Player();
		
	//virtual void play()=0;
	
	private:
		PlayerType m_playerType;
};

#endif
