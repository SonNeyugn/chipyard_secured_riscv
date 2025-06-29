#########################################################################################
# makefile variables for Hammer tutorials
#########################################################################################
tutorial ?= none
EXTRA_CONFS ?=

# TODO: eventually have asap7 commercial/openroad tutorial flavors
ifeq ($(tutorial),asap7)
    tech_name         ?= asap7
    CONFIG            ?= TinyRocketConfig
    TOOLS_CONF        ?= example-tools.yml
    TECH_CONF         ?= example-asap7.yml
    DESIGN_CONFS      ?=
    VLSI_OBJ_DIR      ?= build-asap7-commercial
    INPUT_CONFS       ?= $(TOOLS_CONF) $(TECH_CONF) $(DESIGN_CONFS) $(EXTRA_CONFS)
endif

ifeq ($(tutorial),sky130-commercial)
    tech_name         ?= sky130
    CONFIG            ?= RocketConfig
    TOOLS_CONF        ?= example-tools.yml
    TECH_CONF         ?= example-sky130.yml
    DESIGN_CONFS      ?= example-design.yml
                        # $(if $(filter $(VLSI_TOP),Rocket), \
                        #     example-designs/sky130-rocket.yml, )
    VLSI_OBJ_DIR      ?= build-sky130-commercial
    INPUT_CONFS       ?= $(TOOLS_CONF) $(TECH_CONF) $(DESIGN_CONFS) $(EXTRA_CONFS)
endif

ifeq ($(tutorial),sky130-commercial-quad)
    tech_name         ?= sky130
    CONFIG            ?= QuadRocketConfig
    TOOLS_CONF        ?= example-tools.yml
    TECH_CONF         ?= example-sky130.yml
    DESIGN_CONFS      ?= example-design.yml
                        # $(if $(filter $(VLSI_TOP),Rocket), \
                        #     example-designs/sky130-rocket.yml, )
    VLSI_OBJ_DIR      ?= build-sky130-commercial
    INPUT_CONFS       ?= $(TOOLS_CONF) $(TECH_CONF) $(DESIGN_CONFS) $(EXTRA_CONFS)
endif

ifeq ($(tutorial),sky130-openroad)
    tech_name         ?= sky130
    CONFIG            ?= TinyRocketConfig
    TOOLS_CONF        ?= example-openroad.yml
    TECH_CONF         ?= example-sky130.yml
    DESIGN_CONFS      ?= example-designs/sky130-openroad.yml \
                        $(if $(filter $(VLSI_TOP),Rocket), \
                            example-designs/sky130-rocket.yml) \
                        $(if $(filter $(VLSI_TOP),RocketTile), \
                            example-designs/sky130-openroad-rockettile.yml, )
    VLSI_OBJ_DIR      ?= build-sky130-openroad
    INPUT_CONFS       ?= $(TOOLS_CONF) $(TECH_CONF) $(DESIGN_CONFS) $(EXTRA_CONFS)
    # Yosys compatibility for CIRCT-generated Verilog
    ENABLE_YOSYS_FLOW  = 1
endif
