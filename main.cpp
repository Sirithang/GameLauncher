#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <direct.h>
#include <dirent.h>

#include <vector>
#include <fstream>

struct GameInfo
{
	sf::String name;
	sf::String pathToExecutable;
	sf::String workingDir;
	sf::Texture texture;
	sf::Sprite	sprite;

	void setTexture(sf::String pPath) { texture.loadFromFile(pPath); sprite.setTexture(texture); }
};

std::vector<GameInfo*> sGames; // list of all gams found;
sf::RenderWindow sWin;
int sCurrentIndex = 0;
sf::String		sWorkingDir;

bool fexists(const char* filename)
{
	std::ifstream ifile(filename);
	return ifile;
}

void loadAllGames();
void launchGames();

int main(int argc, char* argv[])
{
	char lDir[1000];
	GetCurrentDirectory(1000, lDir);
	sWorkingDir = lDir;

	loadAllGames();

	if(sGames.size() == 0)
		return -1;

	sf::VideoMode lMode = sf::VideoMode::getDesktopMode();

	sWin.create(lMode, "LAUNCHER", sf::Style::None);
	sWin.setMouseCursorVisible(false);

	sWin.setView(sf::View(	sf::Vector2f(0,0), 
							sf::Vector2f(lMode.width, lMode.height)));

	sf::Text lTxt;
	sf::Text lTitle("Choose Your Game");
	lTitle.setPosition(	lTitle.getGlobalBounds().width * -0.5f,
						lMode.height * -0.5f);

	while(sWin.isOpen())
	{
		
		sf::Event e;
		while(sWin.pollEvent(e))
		{
			switch(e.type)
			{
			case sf::Event::KeyPressed:
				if(e.key.code == sf::Keyboard::Escape)
					sWin.close();
				else if(e.key.code == sf::Keyboard::Left)
				{
					sCurrentIndex = sCurrentIndex - 1 >= 0 ? sCurrentIndex - 1 : sGames.size() - 1;
				}
				else if(e.key.code == sf::Keyboard::Right)
				{
					sCurrentIndex = sCurrentIndex + 1 < sGames.size() ? sCurrentIndex + 1 : 0;
				}
				else if(e.key.code == sf::Keyboard::Return)
				{
					launchGames();
				}
				break;
			case sf::Event::JoystickButtonPressed:
				if(e.joystickButton.joystickId == 0 && e.joystickButton.button == 5)
					sWin.close();
				else if(e.joystickButton.joystickId == 0 && e.joystickButton.button == 0)
				{
					launchGames();
				}
				break;
			case sf::Event::JoystickMoved:
				if(e.joystickMove.joystickId == 0 && e.joystickMove.axis == sf::Joystick::PovX)
				{
					if(e.joystickMove.position < 0)
					{
						sCurrentIndex = sCurrentIndex - 1 >= 0 ? sCurrentIndex - 1 : sGames.size() - 1;
					}
					else if(e.joystickMove.position > 0)
					{
						sCurrentIndex = sCurrentIndex + 1 < sGames.size() ? sCurrentIndex + 1 : 0;
					}
				}
				break;
			case sf::Event::Closed:
				break;
			default:
				break;
			}
		}

		sWin.clear(sf::Color(50, 50, 50, 255));


		if(sCurrentIndex > 0)
		{
			//--sprite
			sGames[sCurrentIndex - 1]->sprite.setScale(0.6f, 0.6f);
			sGames[sCurrentIndex - 1]->sprite.setPosition(	lMode.width * -0.25f - sGames[sCurrentIndex - 1]->sprite.getGlobalBounds().width * 0.5f,
															-sGames[sCurrentIndex - 1]->sprite.getGlobalBounds().height * 0.5f);
			sWin.draw(sGames[sCurrentIndex - 1]->sprite);

			//--name
			lTxt.setString(sGames[sCurrentIndex - 1]->name);
			lTxt.setScale(0.6f, 0.6f);
			lTxt.setPosition(	lMode.width * -0.25f - lTxt.getGlobalBounds().width * 0.5f,
								130);
			
			sWin.draw(lTxt);
		}

		//--sprite
		sGames[sCurrentIndex]->sprite.setScale(1.0f, 1.0f);
		sGames[sCurrentIndex]->sprite.setPosition(	-sGames[sCurrentIndex]->sprite.getGlobalBounds().width * 0.5f,
													-sGames[sCurrentIndex]->sprite.getGlobalBounds().height * 0.5f);
		sWin.draw(sGames[sCurrentIndex]->sprite);
		//--name
		lTxt.setString(sGames[sCurrentIndex]->name);
		lTxt.setScale(1.0f, 1.0f);
		lTxt.setPosition( - lTxt.getGlobalBounds().width * 0.5f,
							230);

		sWin.draw(lTxt);

		if(sCurrentIndex < sGames.size() - 1)
		{
			//--sprite
			sGames[sCurrentIndex + 1]->sprite.setScale(0.6f, 0.6f);
			sGames[sCurrentIndex + 1]->sprite.setPosition(	lMode.width * 0.25f - sGames[sCurrentIndex + 1]->sprite.getGlobalBounds().width * 0.5f,
															-sGames[sCurrentIndex + 1]->sprite.getGlobalBounds().height * 0.5f);
			sWin.draw(sGames[sCurrentIndex + 1]->sprite);

			//--name
			lTxt.setString(sGames[sCurrentIndex + 1]->name);
			lTxt.setScale(0.6f, 0.6f);
			lTxt.setPosition(	lMode.width * 0.25f - lTxt.getGlobalBounds().width * 0.5f,
								130);

			sWin.draw(lTxt);
		}

		sWin.display();
	}

	return 0;
}

//-------------------------------------------


void loadAllGames()
{
	DIR *lDir;
	struct dirent *ent;

	lDir = opendir ("games");

	if (lDir != NULL) 
	{

		/* print all the files and directories within directory */
		while ((ent = readdir (lDir)) != NULL) 
		{
			if(ent->d_type == DT_DIR)
			{
				sf::String lStr(ent->d_name);
				if(lStr == "." || lStr == "..")
				continue;

				GameInfo* lInfo = NULL;
			  
				DIR *lDirExec;
				struct dirent *entExec;

				lDirExec = opendir((sf::String("games/")+ent->d_name).toAnsiString().c_str());

				while ((entExec = readdir (lDirExec)) != NULL) 
				{
					if(entExec->d_type == DT_REG)
					{
						sf::String lStr(entExec->d_name);

						if(lStr.find(".exe") != sf::String::InvalidPos ||lStr.find(".bat") != sf::String::InvalidPos)
						{
							lInfo = new GameInfo();
							lInfo->name = ent->d_name;
							lInfo->pathToExecutable = sf::String("games/")+ent->d_name+"/"+entExec->d_name;
							lInfo->workingDir = sf::String("games/")+ent->d_name;
							
							if(fexists( (sf::String("games/")+ent->d_name+"/miniature.jpg").toAnsiString().c_str()))
							{
								lInfo->setTexture((sf::String("games/")+ent->d_name+"/miniature.jpg"));
							}

							break;
						}
					}
				}

				if(lInfo != NULL)
				{
					sGames.push_back(lInfo);
				}
			  
			}
		}
		closedir (lDir);
	}
}

//-------------------------------------------------------

HWND GetProcessMainWindows (DWORD dwProcessID)
{
	HWND hwnd = NULL;

	do 
	{
		 hwnd = FindWindowEx (NULL, hwnd, NULL, NULL);
		 DWORD dwPID = 0;
		 GetWindowThreadProcessId (hwnd, &dwPID);
		 if (dwPID == dwProcessID)
			return hwnd;
	}
	while (hwnd != NULL);

	return NULL;
}

//-------------------------------------------------------

void launchGames()
{
	STARTUPINFO info={sizeof(info)};
	PROCESS_INFORMATION processInfo;

	info.wShowWindow = TRUE;

	sWin.setVisible(false);

	sWin.setSize(sf::Vector2u(0,0));

	ShowWindow(sWin.getSystemHandle(), SW_HIDE);
	ShowWindow(sWin.getSystemHandle(), SW_HIDE);
	ShowWindow(sWin.getSystemHandle(), SW_HIDE);
	ShowWindow(sWin.getSystemHandle(), SW_HIDE);

	//sf::sleep(sf::milliseconds(2000));

	std::string lStr = sGames[sCurrentIndex]->workingDir.toAnsiString();
	SetCurrentDirectory(lStr.c_str());

	if (CreateProcess((sWorkingDir + "/" + sGames[sCurrentIndex]->pathToExecutable).toAnsiString().c_str(), "", NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
	{
		SetForegroundWindow(GetProcessMainWindows(processInfo.dwProcessId));
		ShowWindow(GetProcessMainWindows(processInfo.dwProcessId), SW_HIDE);
		::WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}

	//sf::sleep(sf::milliseconds(2000));
	sWin.setSize(sf::Vector2u(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height));

	sWin.setVisible(true);

	ShowWindow(sWin.getSystemHandle(), SW_SHOW);
	ShowWindow(sWin.getSystemHandle(), SW_SHOW);

	SetForegroundWindow(sWin.getSystemHandle());
}