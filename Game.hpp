//
//  Game.hpp
//  Checkers
//
//  Created by rick gessner on 2/22/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#ifndef Game_hpp
#define Game_hpp

#include "Player.hpp"
#include "Piece.hpp"
#include <functional>
#include "stdio.h"
#include <iostream>

namespace ECE141 {
  
  enum class Reasons {tbd, forfeit, badmove, eliminated,
                      missedJump, moved2, clockExpired};

  class Game {
  public:
    
    static const size_t kBoardHeight = 8;
    static const size_t kPiecesPerPlayer = 12;
            
    static  Reasons  run(Player &aP1, Player &aP2, std::ostream &anOutput);
    
      //These methods are used by player to interact with game...

    virtual size_t              countAvailablePieces(PieceColor aColor)=0;
    virtual const Piece* const  getAvailablePiece(PieceColor aColor,
                                                  int anIndex)=0;
    virtual const Tile* const   getTileAt(const Location &aLocation)=0;
    virtual bool                movePieceTo(const Piece &aPiece,
                                            const Location &aLocation)=0;
    
    protected:
    
    void setKind(Piece &aPiece, PieceKind aKind);
    void setTile(Piece &aPiece, Tile* aTile);
    void setPiece(Tile &aTile,  Piece* aPiece);

  };
}

#endif /* Game_hpp */
