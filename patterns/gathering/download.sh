#!/bin/bash

MIN_RATING=1400

mkdir -p games
mkdir -p logs

getGameIdsFromTxtList() 
{
	file=$1

	cat $file |grep "#" |grep resign |sed "s/.*#\([0-9]\+\).*/\1/"
}

getRatingsFromGameFile() 
{
	file=$1

	echo `cat $file| grep font |grep "size=3" |grep "color=" | sed "s/.*<b>\([0-9]*\).*/\1/"`
}

# true if >= MIN_RATING
checkRatings() 
{
	a=$1
	b=$2
	test $a -ge $MIN_RATING && test $b -ge $MIN_RATING
}


# http://www.littlegolem.net/jsp/info/player_list.jsp?gtvar=hex_DEFAULT
getPlayersFromFile() 
{
	file=$1;
	cat $file |grep plid |sed "s/.*=\([0-9]*\).*/\1/"  
}


downloadGamesForSinglePlayer() 
{
	player=$1

	wget "http://www.littlegolem.net/jsp/info/player_game_list_txt.jsp?plid=$player&gtid=hex" -O list.txt

	games=`getGameIdsFromTxtList list.txt`

	for game in $games; do
		outFile=game.tmp
		wget "http://www.littlegolem.net/jsp/game/game.jsp?gid=$game" -O $outFile >> logs/wget.log 2>&1
		rating=`getRatingsFromGameFile $outFile`
		flag="skip"
		checkRatings $rating && {
			gameDataFile=games/$game.sgf;
			gameDescFile=games/$game.txt;
			flag="ok";
			echo $game $rating > $gameDescFile;
			wget "http://www.littlegolem.net/servlet/sgf/$game/game.hsgf" -O $gameDataFile >> logs/wget.games.log 2>&1 ;
		};
		echo $flag $game "->" $rating
	done
}

downloadGamesForPlayers()
{
	players=$*
	for player in $players; do 
		echo
		echo "======================================================================="
		echo
		echo Downloading plid $player;
		echo

		downloadGamesForSinglePlayer $player;
	done
}

downloadGamesForPlayers `getPlayersFromFile players/players01.html`
downloadGamesForPlayers `getPlayersFromFile players/players02.html`
downloadGamesForPlayers `getPlayersFromFile players/players03.html`
downloadGamesForPlayers `getPlayersFromFile players/players04.html`
downloadGamesForPlayers `getPlayersFromFile players/players05.html`
downloadGamesForPlayers `getPlayersFromFile players/players06.html`
downloadGamesForPlayers `getPlayersFromFile players/players07.html`
downloadGamesForPlayers `getPlayersFromFile players/players08.html`
downloadGamesForPlayers `getPlayersFromFile players/players09.html`
downloadGamesForPlayers `getPlayersFromFile players/players10.html`
downloadGamesForPlayers `getPlayersFromFile players/players11.html`
downloadGamesForPlayers `getPlayersFromFile players/players12.html`
downloadGamesForPlayers `getPlayersFromFile players/players13.html`
downloadGamesForPlayers `getPlayersFromFile players/players14.html`
downloadGamesForPlayers `getPlayersFromFile players/players15.html`
downloadGamesForPlayers `getPlayersFromFile players/players16.html`



