#################################################################################

 ###############################################################################
 ## Jason Brillante "Damdoshi"                                                ##
 ## Hanged Bunny Studio 2014-2018                                             ##
 ##                                                                           ##
 ## ------------------------------------------------------------------------- ##
 ## ------------------------------------ ---   -   --- ---- --  --   - --- -- ##
 ## ------------------------------------ ---- --- - -- --- - - - -- --  -- -- ##
 ## -----|\-//-------------------------- ---- ---   -- --- - -  --- -- - - -- ##
 ## -----||//___------------------------ ---- --- - -- ---   - ---- -- - - -- ##
 ## ----O  /    \----------------------- ---- --- - -- --- - - ---- -- --  -- ##
 ## ---<________/-----------------------   -   -   ---   - - - ---   - --- -- ##
 ## ------------------------------------------------------------------------- ##
 ##                                                                           ##
 ##                          - LibLapin project -                             ##
 ##                                                                           ##
 ###############################################################################

#################################################################################
## Information about the project                                               ##
#################################################################################

  NAME		=	game.exe
  TITLE		=	"- Magnetic Angel -"
  FREQUENCY	=	30

#################################################################################
## Building details                                                            ##
#################################################################################

  LINKER	?=	gcc
  COMPILER	?=	gcc

  CONFIG	=	-W -Wall -Wno-unused-result -std=c11			\
			-Wno-missing-field-initializers				\
			-DBUNNY_ALLOCATOR_DEACTIVATED

  TEST 		=	-O0 -g -g3 -ggdb -Og -DBUNNY_DEBUG
  PROD 		=	-O2

  WIN		=	-Wl,-subsystem,windows
  TWEAK 	=	$(TEST)

  RM		=	rm -f
  ECHO		=	/bin/echo -e
  LOGFILE	=	errors~
  ERRLOG	=	2>> $(LOGFILE)

  HEADER	=	-I./include						\
			-I${HOME}/.froot/include/				\
			-I/usr/local/include/

  DEFAULT	=	"\033[00m"
  PINK		=	"\033[1;35m"
  GREEN		=	"\033[0;32m"
  TEAL		=	"\033[1;36m"
  RED		=	"\033[0;31m"

#################################################################################
## Source                                                                      ##
#################################################################################

  SRC		=	$(wildcard ./*/*.c)
  OBJ		=	$(SRC:.c=.o)

  LIB		=	-L${HOME}/.froot/lib/ 				\
			-L/usr/local/lib/					\
			-llapin							\
			-lsfml-audio						\
			-lsfml-graphics						\
			-lsfml-window						\
			-lsfml-system						\
			-lm -lavcall -ldl -lstdc++

#################################################################################
## Rules                                                                       ##
#################################################################################

all:			erase $(NAME)
$(NAME):		title $(OBJ)
			@$(LINKER) $(LIB) -o $(NAME) $(OBJ) $(LIB) $(ERRLOG) &&	\
			 $(ECHO) $(TEAL) "[OK]" $(GREEN) $(NAME) $(DEFAULT) ||	\
			 $(ECHO) $(RED)  "[KO]" $(NAME) $(DEFAULT)
.c.o:
			@$(COMPILER) -c $< -o $@ $(TWEAK) $(CONFIG)		\
			 $(HEADER) $(ERRLOG) &&					\
			 $(ECHO) $(TEAL) "[OK]" $(GREEN) $< $(DEFAULT) ||	\
			 $(ECHO) $(RED)  "[KO]" $< $(DEFAULT)

#################################################################################
## Misc                                                                        ##
#################################################################################

title:
			@$(ECHO) $(TEAL) $(TITLE) $(DEFAULT)
clean:
			@find . -name "*.o" -delete &&				\
			 $(ECHO) $(GREEN) "Object file deleted" $(DEFAULT) ||	\
			 $(ECHO) $(RED) "Error in clean rule!" $(DEFAULT)
fclean:			clean
			@$(RM) $(NAME) &&					\
			 $(ECHO) $(GREEN) "Program deleted!" $(DEFAULT) ||	\
			 $(ECHO) $(RED) "Error in fclean rule!" $(DEFAULT)
re:			fclean all
erase:
			@$(RM) $(LOGFILE)
