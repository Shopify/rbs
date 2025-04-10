require "test_helper"

class RBS::InlineAnnotationParsingTest < Test::Unit::TestCase
  include RBS

  include TestHelper

  def test_parse__trailing_assertion
    Parser.parse_inline_trailing_annotation(": String", 0...).tap do |annot|
      assert_instance_of AST::Ruby::Annotations::NodeTypeAssertion, annot
      assert_equal ": String", annot.location.source
      assert_equal ":", annot.prefix_location.source
      assert_equal "String", annot.type.location.source
    end
  end

  def test_error__trailing_assertion
    assert_raises RBS::ParsingError do
      Parser.parse_inline_trailing_annotation(": String[", 0...)
    end

    assert_raises RBS::ParsingError do
      Parser.parse_inline_trailing_annotation(":", 0...)
    end

    assert_raises RBS::ParsingError do
      Parser.parse_inline_trailing_annotation(": String is a ", 0...)
    end
  end

  def test_parse__colon_method_type_annotation
    Parser.parse_inline_leading_annotation(": (String) -> void", 0...).tap do |annot|
      assert_instance_of AST::Ruby::Annotations::ColonMethodTypeAnnotation, annot
      assert_equal ": (String) -> void", annot.location.source
      assert_equal ":", annot.prefix_location.source
      assert_equal "(String) -> void", annot.method_type.location.source
      assert_empty annot.annotations
    end

    Parser.parse_inline_leading_annotation(": %a{a} %a{b} (String) -> void", 0...).tap do |annot|
      assert_instance_of AST::Ruby::Annotations::ColonMethodTypeAnnotation, annot
      assert_equal ": %a{a} %a{b} (String) -> void", annot.location.source
      assert_equal ":", annot.prefix_location.source
      assert_equal "(String) -> void", annot.method_type.location.source
      assert_equal ["a", "b"], annot.annotations.map(&:string)
    end
  end

  def test_parse__rbs_method_types_annotation
    Parser.parse_inline_leading_annotation("@rbs %a{a} (String) -> void", 0...).tap do |annot|
      assert_instance_of AST::Ruby::Annotations::MethodTypesAnnotation, annot
      assert_equal "@rbs %a{a} (String) -> void", annot.location.source
      assert_equal "@rbs", annot.prefix_location.source
      annot.overloads[0].tap do |overload|
        assert_equal "(String) -> void", overload.method_type.location.source
        assert_equal ["a"], overload.annotations.map(&:string)
      end
      assert_empty annot.vertical_bar_locations
    end

    Parser.parse_inline_leading_annotation("@rbs %a{a} (String) -> void | [T] (T) -> T", 0...).tap do |annot|
      assert_instance_of AST::Ruby::Annotations::MethodTypesAnnotation, annot
      assert_equal "@rbs %a{a} (String) -> void | [T] (T) -> T", annot.location.source
      assert_equal "@rbs", annot.prefix_location.source
      annot.overloads[0].tap do |overload|
        assert_equal "(String) -> void", overload.method_type.location.source
        assert_equal ["a"], overload.annotations.map(&:string)
      end
      annot.overloads[1].tap do |overload|
        assert_equal "[T] (T) -> T", overload.method_type.location.source
        assert_equal [], overload.annotations.map(&:string)
      end
      assert_equal ["|"], annot.vertical_bar_locations.map(&:source)
    end
  end
end
