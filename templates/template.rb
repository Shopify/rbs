# frozen_string_literal: true

require "erb"
require "fileutils"
require "yaml"

module RBS
  class Template
    class Field
      attr_reader :name
      attr_reader :c_type

      def initialize(yaml)
        @name = yaml["name"]
        @c_type = "VALUE"
      end
    end

    class Type
      # The fully-qualified name of the auto-generated Ruby class for this type,
      # e.g. `RBS::AST::Declarations::TypeAlias`
      attr_reader :ruby_full_name #: String

      # The name of the name of the auto-generated Ruby class for this type,
      # e.g. `TypeAlias`
      attr_reader :ruby_class_name #: String

      # The name of the auto-generated C struct for this type,
      # e.g. `rbs_ast_declarations_typealias_t`
      attr_reader :c_type_name #: String

      # The name of the pre-existing C function which constructs new Ruby objects of this type.
      # e.g. `rbs_ast_declarations_typealias_new`
      attr_reader :c_function_name #: String

      # The name of the C constant which stores the Ruby VALUE pointing to the generated class.
      # e.g. `RBS_AST_Declarations_TypeAlias`
      attr_reader :c_constant_name #: String

      # The name of the C constant in which the `c_constant_name` is nested.
      # e.g. `RBS_AST_Declarations`
      attr_reader :c_parent_constant_name #: String

      attr_reader :fields #: Array[RBS::Template::Field]

      attr_reader :generate_ruby_obj #: Boolean

      def initialize(yaml)
        @ruby_full_name = yaml["name"]
        @ruby_class_name = @ruby_full_name[/[^:]+\z/] # demodulize-like
        name = @ruby_full_name.gsub("::", "_")
        @c_function_name = name.gsub(/(^)?(_)?([A-Z](?:[A-Z]*(?=[A-Z_])|[a-z0-9]*))/) { ($1 || $2 || "_") + $3.downcase } # underscore-like
        @c_function_name.gsub!(/^rbs_types_/, 'rbs_')
        @c_function_name.gsub!(/^rbs_ast_declarations_/, 'rbs_ast_decl_')
        @c_constant_name = @ruby_full_name.gsub("::", "_")
        @c_parent_constant_name = @ruby_full_name.split("::")[0..-2].join("::").gsub("::", "_")

        @fields = yaml.fetch("fields", []).map { |field| Field.new(field) }.freeze

        @generate_ruby_obj = yaml.fetch("generate_ruby_obj", false)
      end
    end

    class << self
      def render(out_file)
        filepath = "templates/#{out_file}.erb"
        template = File.expand_path("../#{filepath}", __dir__)

        erb = read_template(template)
        extension = File.extname(filepath.gsub(".erb", ""))

        heading = <<~HEADING
          /*----------------------------------------------------------------------------*/
          /* This file is generated by the templates/template.rb script and should not  */
          /* be modified manually.                                                      */
          /* To change the template see                                                 */
          /* #{filepath + " " * (74 - filepath.size) } */
          /*----------------------------------------------------------------------------*/
        HEADING

        write_to = File.expand_path("../#{out_file}", __dir__)
        contents = heading + "\n" + erb.result_with_hash(locals)

        if (extension == ".c" || extension == ".h") && !contents.ascii_only?
          # Enforce that we only have ASCII characters here. This is necessary
          # for non-UTF-8 locales that only allow ASCII characters in C source
          # files.
          contents.each_line.with_index(1) do |line, line_number|
            raise "Non-ASCII character on line #{line_number} of #{write_to}" unless line.ascii_only?
          end
        end

        FileUtils.mkdir_p(File.dirname(write_to))
        File.write(write_to, contents)
      end

      private

      def read_template(filepath)
        template = File.read(filepath, encoding: Encoding::UTF_8)
        erb = erb(template)
        erb.filename = filepath
        erb
      end

      def erb(template)
        ERB.new(template, trim_mode: "-")
      end

      def locals
        config = YAML.load_file(File.expand_path("../config.yml", __dir__))
        {
          nodes: config.fetch("nodes").map { |node| Type.new(node) }.sort_by(&:ruby_full_name),
        }
      end
    end
  end
end

unless ARGV.size == 1
  $stderr.puts "Usage: ruby template.rb <out_file>"
  exit 1
end

out_file = ARGV.first
RBS::Template.render(out_file)
