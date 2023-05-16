#include "Player.hpp"
#include <vector>

namespace ECE141 {
    enum class moveType {jump, move};
    class Game;
        struct move {
        moveType type;
        std::vector<Location> locList;
        std::vector<std::vector<int>> projection;
    };

    class YaPlayer : public Player {
        public:
            YaPlayer();
            bool takeTurn(Game &aGame, Orientation aDirection, std::ostream &aLog);
            void setPlane(std::vector<std::vector<int>> &plane, Game & aGame);
            bool canJumpTo(std::vector<std::vector<int>> &plane, const Piece * aPiece, Location aLoc);
            void checkMoveTo(std::vector<std::vector<int>> &plane, const Piece * thePiece, 
                                std::vector<move> &jumpList, std::vector<move> &moveList, int row, int col);
            bool recursiveJump(std::vector<std::vector<int>> &plane, const Piece * thePiece, 
                        std::vector<move> &list, std::vector<Location> &locList, int row, int col);
            bool safe(std::vector<std::vector<int>>& plane, Location loc);
            float value(move aMove, PieceKind type);
            float distVal(move aMove);

    };
}