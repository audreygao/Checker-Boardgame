#include "YaPlayer.hpp"
#include "Game.hpp"
#include <vector>
#include <algorithm>
#include <math.h>
#include <stdlib.h>

namespace ECE141 {
    YaPlayer::YaPlayer() : Player() {}

    void printPlane(std::vector<std::vector<int>> &plane) {
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                std::cout << plane.at(i).at(j);
            }
            std::cout << "\n";
        }
    }

    void YaPlayer::setPlane(std::vector<std::vector<int>> &plane, Game & aGame) {
        for(int row=0; row < 8; row++) {
            std::vector<int> innerPlane;
            for(int col=0; col < 8; col++) {

                Location aLocation = Location(row, col);
                const Piece * aPiece = aGame.getTileAt(aLocation)->getPiece();
                if(aPiece==nullptr) {
                    innerPlane.push_back(0);
                } else if(aPiece->getColor()==PieceColor::blue) {
                    if(aPiece->getKind()==PieceKind::king) innerPlane.push_back(3);
                    if(aPiece->getKind()==PieceKind::pawn) innerPlane.push_back(1);
                } else {
                    if(aPiece->getKind()==PieceKind::king) innerPlane.push_back(4);
                    if(aPiece->getKind()==PieceKind::pawn) innerPlane.push_back(2);
                }
                //std::cout << innerPlane.at(col);
                
            }
            //std::cout << "\n";
            plane.push_back(innerPlane);
        }
    }

    Location moveBy(const Piece * aPiece, int row, int col) {
        int oldRow = aPiece->getLocation().row;
        int oldCol = aPiece->getLocation().col;
        if(aPiece->getColor() == PieceColor::gold) {
            row = -1 * row;
            col = -1 * col;
        }
        return Location(oldRow + row, oldCol + col);
    }

    bool canMoveTo(std::vector<std::vector<int>> &plane, Location aLoc) {
        int row = aLoc.row;
        int col = aLoc.col;

        //check that the move to locatio is within range of gameboard
        if(row >= 0 && row < 8 && col >= 0 && col < 8) {
            if(plane.at(row).at(col) == 0) {
                return true;
            }
        }
        return false;
    }

    bool YaPlayer::canJumpTo(std::vector<std::vector<int>> &plane, const Piece * aPiece, Location aLoc) {

        //check if the toLocation is empty and within bounds
        if(canMoveTo(plane, aLoc) ) {
            int rowDiff = (aLoc.row - aPiece->getLocation().row) / 2;
            int colDiff = (aLoc.col - aPiece->getLocation().col) / 2;
            Location over = Location(aPiece->getLocation().row+rowDiff, aPiece->getLocation().col+colDiff);

            //check if the between piece exists and is opponent
            int fakePiece = plane.at(over.row).at(over.col);
            if(color==PieceColor::blue && (fakePiece == 2 || fakePiece == 4)) return true;
            if(color==PieceColor::gold && (fakePiece == 1 || fakePiece == 3)) return true;
        }
        return false;
    }

    bool YaPlayer::recursiveJump(std::vector<std::vector<int>> &plane, const Piece * thePiece, 
                        std::vector<move> &list, std::vector<Location> &locList, int row, int col) {
        std::vector<std::vector<int>> projection = plane;
        
        Location toLoc = moveBy(thePiece, row, col);
        if(!canJumpTo(plane, thePiece, toLoc)) {return false;} 

        //set the projection
        Location capture = moveBy(thePiece, row/2, col/2);
        projection.at(toLoc.row).at(toLoc.col) = projection.at(thePiece->getLocation().row).at(thePiece->getLocation().col);
        projection.at(thePiece->getLocation().row).at(thePiece->getLocation().col) = 0;
        projection.at(capture.row).at(capture.col) = 0;

        //add the piece location to locList
        Tile * fakeTile = new Tile(TileColor::dark, toLoc);
        Piece * fakeNext;
        if(toLoc.row ==0 || toLoc.row==7) fakeNext = new Piece(color, fakeTile, PieceKind::king);
        else fakeNext = new Piece(color, fakeTile, thePiece->getKind());
        locList.push_back(toLoc);

        bool a=false;
        bool b=false;
        bool c,d;

        //recurse in the four directions
        c = recursiveJump(projection, fakeNext, list, locList, -2, -2);
        d = recursiveJump(projection, fakeNext, list, locList, -2, 2);
        if(thePiece->getKind()==PieceKind::king) {
            b = recursiveJump(projection, fakeNext, list, locList, 2, 2);
            a = recursiveJump(projection, fakeNext, list, locList, 2, -2);
        }
        

        
        if(!a && !b && !c && !d) {
            //end of this jump, make a move with the currentLocList and add the move to list, pop his location from locList
            list.push_back(move{moveType::jump, locList, projection});
        }
        delete fakeTile;
        delete fakeNext;
        locList.pop_back();
        return true;
    }

    void YaPlayer::checkMoveTo(std::vector<std::vector<int>> &plane, const Piece * thePiece, 
                                std::vector<move> &jumpList, std::vector<move> &moveList, int row, int col) {
        Location toLoc = moveBy(thePiece, row, col);
        if(canJumpTo(plane, thePiece, toLoc)) {
            std::vector<Location> locList;
            locList.push_back(thePiece->getLocation());
            recursiveJump(plane, thePiece, jumpList, locList, row, col);
                        //add the move to JumpList
        } else {
            toLoc = moveBy(thePiece, row/2, col/2);
            if(canMoveTo(plane, toLoc)) {
                std::vector<Location> locList;
                locList.push_back(thePiece->getLocation());
                locList.push_back(toLoc);
                std::vector<std::vector<int>> projection = plane;
                projection.at(toLoc.row).at(toLoc.col) = projection.at(thePiece->getLocation().row).at(thePiece->getLocation().col);
                projection.at(thePiece->getLocation().row).at(thePiece->getLocation().col) = 0;
                moveList.push_back(move{moveType::move, locList, projection});
                            //add the move to MoveList;
            }
        }
    }
    bool inRange(int row, int col) {
        return row >= 0 && row < 8 && col >= 0 && col < 8;
    }

    bool YaPlayer::safe(std::vector<std::vector<int>>& plane, Location loc) {
        bool result=false;
        if(inRange(loc.row - 1, loc.col - 1) && inRange(loc.row + 1, loc.col + 1)) {

            int topLeft = plane.at(loc.row-1).at(loc.col-1);
            int lowerRight = plane.at(loc.row+1).at(loc.col+1);
            result =  color==PieceColor::blue? 
                        ((topLeft==2 || topLeft==4) && lowerRight==0 ) || (lowerRight==4 && topLeft==0 ) : 
                        ((topLeft==1 || topLeft==3) && lowerRight==0 ) || (lowerRight==3 && topLeft==0 );
            return result;
        }
        if(inRange(loc.row - 1, loc.col + 1) && inRange(loc.row + 1, loc.col - 1)) {
            int topRight = plane.at(loc.row-1).at(loc.col+1);
            int lowerLeft = plane.at(loc.row+1).at(loc.col-1);
            result =  color==PieceColor::blue? 
                            ((topRight==2 || topRight==4) && lowerLeft==0 ) || (lowerLeft==4 && topRight==0 ) :
                            ((topRight==1 || topRight==3) && lowerLeft==0 ) || (lowerLeft==3 && topRight==0 );
            return result;
        }
        return false;
    }

    float distance(int r1, int c1, int r2, int c2) {
        return sqrt((r1-r2)*(r1-r2)+(c1-c2)*(c1-c2));
    }

    float YaPlayer::distVal(move aMove) {
        Location startLoc = aMove.locList.at(0);
        Location endLoc = aMove.locList.at(aMove.locList.size()-1);
        float minDist = 100;
        for(int row=0; row < 8; row++) {
            for(int col=0; col < 8; col++) {
                int a = (color==PieceColor::blue)? 2:1;
                int b = (color==PieceColor::blue)? 4:3;
                if(aMove.projection.at(row).at(col)==a || aMove.projection.at(row).at(col)==b) {
                    float d = distance(endLoc.row, endLoc.col, row, col);
                    if( d < minDist) {
                        minDist = d;
                        
                    }
                }
            }
        }
        return minDist==100? minDist : 10 - minDist;
    }

    float YaPlayer::value(move aMove, PieceKind type) {
        int val = aMove.locList.size();
        Location at = aMove.locList.at(aMove.locList.size()-1);
        if(safe(aMove.projection, at)) {
            val = val + 20;
        }
        if(type==PieceKind::pawn) val+=1;
        val = val + distVal(aMove) + 1;
        return val;
    }


    bool YaPlayer::takeTurn(Game &aGame, Orientation aDirection, std::ostream &aLog) {
        //construct the current gameboard
        std::vector<std::vector<int>> plane;
        setPlane(plane, aGame);

        //list of available moves
        std::vector<move> jumpList, moveList, theList;

        size_t theCount=aGame.countAvailablePieces(color);
            for(int pos=0;pos<theCount;pos++) {
                if(const Piece *thePiece = aGame.getAvailablePiece(this->color, pos)) {

                    //randomize order
                    if(rand() % 2) {
                    checkMoveTo(plane, thePiece, jumpList, moveList, -2, -2);//forwardLeft
                    checkMoveTo(plane, thePiece, jumpList, moveList, -2, 2);//forwardRight
                    } else {
                        checkMoveTo(plane, thePiece, jumpList, moveList, -2, 2);//forwardRight
                        checkMoveTo(plane, thePiece, jumpList, moveList, -2, -2);//forwardLeft
                        
                    }
                    if(thePiece->getKind()==PieceKind::king) {

                        //randomize order
                        if(rand() % 2) {
                        checkMoveTo(plane, thePiece, jumpList, moveList, 2, 2);//backwardRight
                        checkMoveTo(plane, thePiece, jumpList, moveList, 2, -2);//backwardLeft
                        } else {
                            checkMoveTo(plane, thePiece, jumpList, moveList, 2, -2);//backwardLeft
                            checkMoveTo(plane, thePiece, jumpList, moveList, 2, 2);//backwardRight
                        }
                    }
                }
            }
            //loop through the list of moves, finding the move with the highest value
            theList = jumpList;
            float maxValue = 0;
            std::vector<Location> locList;
            move decision = {moveType::move, locList};
            if(jumpList.empty()) theList = moveList;
            for(auto move : theList) {
                float val = value(move, aGame.getTileAt(move.locList.at(0))->getPiece()->getKind());
                if(val > maxValue) {
                    decision = move;
                    maxValue = val;
                }
            }

        //iterate through locList to be move to in this turn
        const Piece * thePiece = (aGame.getTileAt(decision.locList.at(0))->getPiece());
        for(int i = 0; i < decision.locList.size(); i++) {
            aGame.movePieceTo(*thePiece, decision.locList.at(i));
        }
        //printPlane(decision.projection);
        //std::cout << "------------------------------------------------\n";
        return true;
    }
}