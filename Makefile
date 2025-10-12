# Makefile pour Kyber/ML-KEM NTT Implementation
# Auteur: Gabriel Abauzit

# Compilateur et flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lm

# Répertoires
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
BUILD_DIR = build

# Fichiers sources
SOURCES = $(SRC_DIR)/ntt.c $(SRC_DIR)/poly.c $(SRC_DIR)/polyvec.c
HEADERS = $(INC_DIR)/ntt.h $(INC_DIR)/poly.h $(INC_DIR)/polyvec.h
TEST_SOURCES = $(TEST_DIR)/test_ntt.c

# Fichiers objets
OBJECTS = $(BUILD_DIR)/ntt.o $(BUILD_DIR)/poly.o $(BUILD_DIR)/polyvec.o
TEST_OBJECTS = $(BUILD_DIR)/test_ntt.o

# Exécutables
TEST_EXEC = test_ntt

# Couleurs pour l'affichage
GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m

# Règles phony
.PHONY: all clean test help dirs

# Règle par défaut
all: dirs $(TEST_EXEC)
	@echo "$(GREEN)✓ Compilation terminée avec succès$(NC)"

# Créer les répertoires nécessaires
dirs:
	@mkdir -p $(BUILD_DIR)

# Compilation de l'exécutable de test
$(TEST_EXEC): $(OBJECTS) $(TEST_OBJECTS)
	@echo "$(YELLOW)🔗 Liaison de $@...$(NC)"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation des fichiers objets de src/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@echo "$(YELLOW)🔨 Compilation de $<...$(NC)"
	$(CC) $(CFLAGS) -c $< -o $@

# Compilation des fichiers objets de tests/
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c $(HEADERS)
	@echo "$(YELLOW)🔨 Compilation de $<...$(NC)"
	$(CC) $(CFLAGS) -c $< -o $@

# Exécution des tests
test: $(TEST_EXEC)
	@echo "$(YELLOW)🧪 Exécution des tests...$(NC)"
	@./$(TEST_EXEC)
	@if [ $$? -eq 0 ]; then \
		echo "$(GREEN)✅ Tous les tests sont passés !$(NC)"; \
	else \
		echo "$(RED)❌ Certains tests ont échoué.$(NC)"; \
		exit 1; \
	fi

# Tests rapides (100 essais)
test-quick: clean
	@echo "$(YELLOW)🧪 Tests rapides (100 essais)...$(NC)"
	@$(MAKE) CFLAGS="$(CFLAGS) -DNBRE_ESSAIS=100" $(TEST_EXEC)
	@./$(TEST_EXEC)

# Tests complets (1000 essais)
test-full: clean
	@echo "$(YELLOW)🧪 Tests complets (1000 essais)...$(NC)"
	@$(MAKE) CFLAGS="$(CFLAGS) -DNBRE_ESSAIS=1000" $(TEST_EXEC)
	@./$(TEST_EXEC)

# Nettoyage
clean:
	@echo "$(YELLOW)🧹 Nettoyage...$(NC)"
	rm -rf $(BUILD_DIR) $(TEST_EXEC)
	@echo "$(GREEN)✓ Nettoyage terminé$(NC)"

# Aide
help:
	@echo "$(GREEN)═══════════════════════════════════════════════$(NC)"
	@echo "$(GREEN)  Makefile pour NTT Kyber/ML-KEM$(NC)"
	@echo "$(GREEN)═══════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "Commandes disponibles:"
	@echo "  $(YELLOW)make$(NC)              - Compile le projet"
	@echo "  $(YELLOW)make test$(NC)         - Compile et exécute les tests (1000 essais)"
	@echo "  $(YELLOW)make test-quick$(NC)   - Tests rapides (100 essais)"
	@echo "  $(YELLOW)make test-full$(NC)    - Tests complets (1000 essais)"
	@echo "  $(YELLOW)make clean$(NC)        - Supprime les fichiers générés"
	@echo "  $(YELLOW)make help$(NC)         - Affiche cette aide"
	@echo ""
	@echo "Exemples d'utilisation:"
	@echo "  $$ make clean"
	@echo "  $$ make test"
	@echo ""
	@echo "Structure du projet:"
	@echo "  src/        - Fichiers sources (.c)"
	@echo "  include/    - Headers (.h)"
	@echo "  tests/      - Tests unitaires"
	@echo "  build/      - Fichiers objets compilés"