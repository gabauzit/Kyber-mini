# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS = 

# Répertoires
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
OBJ_DIR = build

# Fichiers source
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Fichiers de test
TEST_NTT_SRC = $(TEST_DIR)/test_ntt.c
TEST_NTT_BIN = test_ntt

# Cible par défaut
all: $(OBJS)
	@echo "Compilation des fichiers sources terminée"

# Règle pour créer le répertoire obj
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compilation des fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Cible pour le test NTT
test_ntt: $(OBJS) $(TEST_NTT_SRC)
	$(CC) $(CFLAGS) $(TEST_NTT_SRC) $(OBJS) -o $(TEST_NTT_BIN) $(LDFLAGS)
	./$(TEST_NTT_BIN)

# Nettoyage
clean:
	rm -rf $(OBJ_DIR) $(TEST_NTT_BIN)

# Nettoyage complet
mrproper: clean
	rm -f *~ $(SRC_DIR)/*~ $(INC_DIR)/*~ $(TEST_DIR)/*~

# Afficher l'aide
help:
	@echo "Available commands :"
	@echo "  all            - Compile all source files (default)"
	@echo "  test_ntt       - Compile and run the NTT test"
	@echo "  clean          - Deletes object files and executables"
	@echo "  mrproper       - Complete cleaning"
	@echo "  help           - Display this help"

.PHONY: all test_ntt clean mrproper help