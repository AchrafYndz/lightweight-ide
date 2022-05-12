#ifndef IDE_ID_H
#define IDE_ID_H

using ID = unsigned int;

/// auto increment class complete with an 'id' type definition
class Id {
    ID counter = 0;

  public:
    Id() = default;

    /// returns a new unique id by incrementing the counter and returning the old one
    ID next();
};

#endif  // IDE_ID_H