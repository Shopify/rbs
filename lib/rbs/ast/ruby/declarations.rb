# frozen_string_literal: true

module RBS
  module AST
    module Ruby
      module Declarations
        class Base
          attr_reader :buffer

          include Helpers::ConstantHelper
          include Helpers::LocationHelper

          def initialize(buffer)
            @buffer = buffer
          end
        end

        class ClassDecl < Base
          class SuperClass
            attr_reader :type_name_location

            attr_reader :operator_location

            attr_reader :type_name

            attr_reader :type_annotation

            def initialize(type_name_location, operator_location, type_name, type_annotation)
              @type_name_location = type_name_location
              @operator_location = operator_location
              @type_name = type_name
              @type_annotation = type_annotation
            end

            def type_args
              if type_annotation
                type_annotation.type_args
              else
                []
              end
            end

            def location
              if type_annotation
                Location.new(
                  type_name_location.buffer,
                  type_name_location.start_pos,
                  type_annotation.location.end_pos
                )
              else
                type_name_location
              end
            end

            alias name type_name
            alias args type_args
          end

          attr_reader :class_name

          attr_reader :members

          attr_reader :node

          attr_reader :super_class

          def initialize(buffer, name, node, super_class)
            super(buffer)
            @class_name = name
            @node = node
            @members = []
            @super_class = super_class
          end

          def each_decl(&block)
            return enum_for(:each_decl) unless block

            @members.each do |member|
              if member.is_a?(Base)
                yield member
              end
            end
          end

          def type_params = []

          def location
            rbs_location(node.location)
          end

          def name_location
            rbs_location(node.constant_path.location)
          end
        end

        class ModuleDecl < Base
          attr_reader :module_name

          attr_reader :members

          attr_reader :node

          def initialize(buffer, name, node)
            super(buffer)
            @module_name = name
            @node = node
            @members = []
          end

          def each_decl(&block)
            return enum_for(:each_decl) unless block

            @members.each do |member|
              if member.is_a?(Base)
                yield member
              end
            end
          end

          def type_params = []

          def self_types = []

          def location
            rbs_location(node.location)
          end

          def name_location
            rbs_location(node.constant_path.location)
          end
        end

        class ConstantDecl < Base
          attr_reader :leading_comment
          attr_reader :constant_name
          attr_reader :node
          attr_reader :type_annotation

          def initialize(buffer, constant_name, node, leading_comment, type_annotation)
            super(buffer)
            @constant_name = constant_name
            @node = node
            @leading_comment = leading_comment
            @type_annotation = type_annotation
          end

          def location
            rbs_location(node.location)
          end

          def name_location
            case node
            when Prism::ConstantWriteNode
              rbs_location(node.name_loc)
            when Prism::ConstantPathWriteNode
              rbs_location(node.target.location)
            end
          end

          def type
            return type_annotation.type if type_annotation

            case node.value
            when Prism::IntegerNode
              BuiltinNames::Integer.instance_type
            when Prism::FloatNode
              BuiltinNames::Float.instance_type
            when Prism::StringNode
              BuiltinNames::String.instance_type
            when Prism::TrueNode, Prism::FalseNode
              Types::Bases::Bool.new(location: nil)
            when Prism::SymbolNode
              BuiltinNames::Symbol.instance_type
            when Prism::NilNode
              Types::Bases::Nil.new(location: nil)
            else
              Types::Bases::Any.new(location: nil)
            end
          end

          def comment
            leading_comment&.as_comment
          end
        end
      end
    end
  end
end
